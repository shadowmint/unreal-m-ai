// Fill out your copyright notice in the Description page of Project Settings.


#include "Features/Patrol/MAiTask_PatrolAlongPathCommand.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Features/Patrol/MAiPatrolPath.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMAiTask_PatrolAlongPathCommand::OnStart_Implementation(UObject* Context)
{
	Blackboard->SetValueAsFloat(Config->SpreadKeyFloat.SelectedKeyName, 0);

	// Enable avoidance
	auto Pawn = Controller->GetPawn();
	if (Pawn)
	{
		auto Movement = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
		if (Movement)
		{
			Movement->SetAvoidanceEnabled(true);
		}
	}
}

EMTaskState UMAiTask_PatrolAlongPathCommand::OnPoll_Implementation(float DeltaTime)
{
	if (State != EMTaskState::Running) return State;
	if (!IsValid) return EMTaskState::Rejected;

	auto const ThisPawn = Controller->GetPawn();
	if (!ThisPawn)
	{
		return EMTaskState::Rejected;
	}

	// Update left right sequencing offset
	auto SeqOffset = Blackboard->GetValueAsFloat(Config->SpreadKeyFloat.SelectedKeyName);
	SeqOffset += DeltaTime;
	Blackboard->SetValueAsFloat(Config->SpreadKeyFloat.SelectedKeyName, SeqOffset);

	// Update the last vector
	auto const PawnPos = ThisPawn->GetActorLocation();
	auto LastPawnPosition = Blackboard->GetValueAsVector(Config->LastPositionKeyVector.SelectedKeyName);
	auto StuckTime = Blackboard->GetValueAsFloat(Config->StuckTimeKeyFloat.SelectedKeyName);
	Blackboard->SetValueAsVector(Config->LastPositionKeyVector.SelectedKeyName, PawnPos);
	auto LastPawnStep = (LastPawnPosition - PawnPos).Size();
	if (LastPawnStep < 0.1)
	{
		StuckTime += DeltaTime;
		Blackboard->SetValueAsFloat(Config->StuckTimeKeyFloat.SelectedKeyName, StuckTime);
	}
	else
	{
		Blackboard->SetValueAsFloat(Config->StuckTimeKeyFloat.SelectedKeyName, 0);
	}

	// Are we stuck?
	const auto Stuck = StuckTime > Config->StuckTimeout;

	// If we're busy moving, then resolve that first
	if (Controller->IsFollowingAPath())
	{
		auto StepTimer = Blackboard->GetValueAsFloat(Config->StepTimerKeyFloat.SelectedKeyName);
		if (Stuck || StepTimer > Config->StepTimer)
		{
			Controller->StopMovement();
		}
		else
		{
			Blackboard->SetValueAsFloat(Config->StepTimerKeyFloat.SelectedKeyName, DeltaTime);
			return State;
		}
	}

	auto Target = Cast<AMAiPatrolPath>(Blackboard->GetValueAsObject(Config->PathKeyObject.SelectedKeyName));
	if (!Target)
	{
		return EMTaskState::Rejected;
	}

	// Pick the destination point along the spline	
	auto SplinePosition = Target->Path->FindLocationClosestToWorldLocation(PawnPos, ESplineCoordinateSpace::World);
	auto SplineVector = Target->Path->FindDirectionClosestToWorldLocation(SplinePosition, ESplineCoordinateSpace::World);

	// If we're stuck, pick a different random point to try to unstuck
	if (Stuck)
	{
		auto RandomUnStuckOffset = FMath::FRandRange(0, 1);
		SplinePosition = Target->Path->GetLocationAtTime(RandomUnStuckOffset, ESplineCoordinateSpace::World);
		SplineVector = Target->Path->GetDirectionAtTime(RandomUnStuckOffset, ESplineCoordinateSpace::World);
		Blackboard->SetValueAsFloat(Config->StuckTimeKeyFloat.SelectedKeyName, 0);
	}

	// Figure out the destination
	const auto Destination = SplinePosition + SplineVector * Config->StepSize * DeltaTime;

	// Apply random wandering on path
	auto const Up = ThisPawn->GetActorUpVector();
	auto const OffsetDir = FVector::CrossProduct(Up, SplineVector);

	// A periodic offset
	const auto OffsetValue = FMath::Sin(SeqOffset * Config->SpreadFrequency);
	const auto OffsetSize = OffsetValue * Config->SpreadRadius;
	auto const ActualOffset = OffsetSize * OffsetDir;
	auto const IdealDestination = Destination + ActualOffset;
	auto RealDestination = IdealDestination;

	FNavLocation RandomPoint(IdealDestination);
	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (NavSys)
	{
		ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
		if (UseNavData)
		{
			auto Query = FPathFindingQuery();
			Query.StartLocation = PawnPos;
			Query.EndLocation = RealDestination;
			if (!NavSys->TestPathSync(Query, EPathFindingMode::Regular))
			{
				// If there's no link between the points, move randomly
				auto QueryFilter = UNavigationQueryFilter::GetQueryFilter(*UseNavData, this, nullptr);
				if (NavSys->GetRandomPointInNavigableRadius(PawnPos, Config->SpreadRadius, RandomPoint, UseNavData, nullptr))
				{
					RealDestination = RandomPoint.Location;
				}
			}
		}
	}

	const auto Result = Controller->MoveToLocation(RealDestination, Config->AcceptanceThreshold, false);
	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EMTaskState::Rejected;
	}

	Blackboard->SetValueAsFloat(Config->StepTimerKeyFloat.SelectedKeyName, 0);
	return State;
}
