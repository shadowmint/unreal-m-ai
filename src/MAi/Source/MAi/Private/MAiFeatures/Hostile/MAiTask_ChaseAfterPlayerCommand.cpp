// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/Hostile/MAiTask_ChaseAfterPlayerCommand.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

void UMAiTask_ChaseAfterPlayerCommand::SetAttackSpeed() const
{
	// Assume we're using walking; in the future could switch over movement modes here
	auto const Pawn = Controller->GetPawn();
	if (!Pawn) return;

	auto const PawnMovement = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!PawnMovement) return;

	auto const OldSpeed = PawnMovement->MaxWalkSpeed;
	Blackboard->SetValueAsFloat(Config->OldSpeedKeyFloat.SelectedKeyName, OldSpeed);

	auto const NewSpeed = Config->SpeedBoostWhileChasingFactor * OldSpeed;
	PawnMovement->MaxWalkSpeed = NewSpeed;
}

void UMAiTask_ChaseAfterPlayerCommand::RevertAttackSpeed() const
{
	// Assume we're using walking; in the future could switch over movement modes here
	auto const Pawn = Controller->GetPawn();
	if (!Pawn) return;

	auto const PawnMovement = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!PawnMovement) return;

	auto const OldSpeed = Blackboard->GetValueAsFloat(Config->OldSpeedKeyFloat.SelectedKeyName);
	PawnMovement->MaxWalkSpeed = OldSpeed;
}

void UMAiTask_ChaseAfterPlayerCommand::OnStart_Implementation(UObject* Context)
{
	SetAttackSpeed();
}

void UMAiTask_ChaseAfterPlayerCommand::UpdateMovementDestination(FVector PawnPoint, FVector TargetPoint, UNavigationSystemV1* NavSys)
{
	ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (UseNavData)
	{
		FNavLocation NavPoint(TargetPoint);
		NavSys->GetRandomPointInNavigableRadius(TargetPoint, 0, NavPoint, UseNavData, nullptr);

		FNavLocation RandomPoint(NavPoint.Location);
		auto Query = FPathFindingQuery();
		Query.StartLocation = PawnPoint;
		Query.EndLocation = NavPoint.Location;

		// If there's no link between the points, move randomly
		if (!NavSys->TestPathSync(Query, EPathFindingMode::Regular))
		{
			auto QueryFilter = UNavigationQueryFilter::GetQueryFilter(*UseNavData, this, nullptr);
			if (NavSys->GetRandomPointInNavigableRadius(PawnPoint, Config->AvoidProblemsSpreadRadius, RandomPoint, UseNavData, nullptr))
			{
				Controller->MoveToLocation(RandomPoint.Location, Config->AcceptanceThreshold, false);
			}
			else
			{
				// Do nothing, it's fine. We're stuck.
			}
		}

		// Move to player otherwise
		else
		{
			Controller->MoveToLocation(NavPoint.Location, Config->AcceptanceThreshold, false);
		}
	}
}

EMTaskState UMAiTask_ChaseAfterPlayerCommand::OnPoll_Implementation(float DeltaTime)
{
	if (State != EMTaskState::Running) return State;
	if (!IsValid) return EMTaskState::Rejected;

	auto const ThisPawn = Controller->GetPawn();
	if (!ThisPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("No pawn for chase"));
		return EMTaskState::Rejected;
	}
	auto const PawnPoint = ThisPawn->GetActorLocation();

	// Find the target
	auto const TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(Config->PlayerTargetKeyObject.SelectedKeyName));
	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("No actor for chase"));
		return EMTaskState::Rejected;
	}
	auto const TargetPoint = TargetActor->GetActorLocation();

	// Find nav system
	const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!NavSys)
	{
		UE_LOG(LogTemp, Warning, TEXT("No nav for chase"));
		return EMTaskState::Rejected;
	}

	// Check if we arrived at the player?
	auto DistanceToTarget = (TargetPoint - PawnPoint).Size();
	if (DistanceToTarget < Config->AvoidProblemsSpreadRadius)
	{
		return EMTaskState::Resolved;
	}

	// If we're not following a path, start moving towards actor
	if (!Controller->IsFollowingAPath())
	{
		UpdateMovementDestination(PawnPoint, TargetPoint, NavSys);
		return EMTaskState::Running;
	}

	// If we got here we are already following a path
	auto const Path = Controller->GetPathFollowingComponent();
	if (Path)
	{
		const auto UltimateDestination = Path->GetPathDestination();
		const auto Diff = (UltimateDestination - TargetPoint).Size();
		if (Diff > Config->AvoidProblemsSpreadRadius)
		{
			// The actor moved, update the destination
			UpdateMovementDestination(PawnPoint, TargetPoint, NavSys);
			return EMTaskState::Running;
		}
	}

	return State;
}

void UMAiTask_ChaseAfterPlayerCommand::OnEnd_Implementation()
{
	RevertAttackSpeed();
}
