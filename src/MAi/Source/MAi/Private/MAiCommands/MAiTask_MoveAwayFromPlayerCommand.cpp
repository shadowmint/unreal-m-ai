// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiCommands/MAiTask_MoveAwayFromPlayerCommand.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMAiTask_MoveAwayFromPlayerCommand::SetMoveSpeed() const
{
	// Assume we're using walking; in the future could switch over movement modes here
	auto const Pawn = Controller->GetPawn();
	if (!Pawn) return;

	auto const PawnMovement = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!PawnMovement) return;

	auto const OldSpeed = PawnMovement->MaxWalkSpeed;
	Blackboard->SetValueAsFloat(Config->OldSpeedKeyFloat.SelectedKeyName, OldSpeed);

	auto const NewSpeed = Config->SpeedBoostWhileMovingFactor * OldSpeed;
	PawnMovement->MaxWalkSpeed = NewSpeed;
}

void UMAiTask_MoveAwayFromPlayerCommand::RevertMoveSpeed() const
{
	// Assume we're using walking; in the future could switch over movement modes here
	auto const Pawn = Controller->GetPawn();
	if (!Pawn) return;

	auto const PawnMovement = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	if (!PawnMovement) return;

	auto const OldSpeed = Blackboard->GetValueAsFloat(Config->OldSpeedKeyFloat.SelectedKeyName);
	PawnMovement->MaxWalkSpeed = OldSpeed;
}

void UMAiTask_MoveAwayFromPlayerCommand::OnStart_Implementation(UObject* Context)
{
	SetMoveSpeed();
}

void UMAiTask_MoveAwayFromPlayerCommand::UpdateMovementDestination(FVector PawnPoint, FVector ActorPoint, UNavigationSystemV1* NavSys)
{
	// So, the destination we want is at least TargetDistance from the actor.
	// But don't move *towards* the actor to get there, so pick the furthest point we can from here.
	auto Step = (PawnPoint - ActorPoint);
	Step.Normalize();

	const auto TargetPoint = ActorPoint + Step * Config->TargetDistance + Config->AvoidProblemsSpreadRadius;

	ANavigationData* UseNavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (UseNavData)
	{
		FNavLocation RandomPoint(TargetPoint);
		// Find a target to move to
		if (NavSys->GetRandomPointInNavigableRadius(TargetPoint, Config->AvoidProblemsSpreadRadius, RandomPoint, UseNavData, nullptr))
		{
			Controller->MoveToLocation(RandomPoint.Location, Config->AcceptanceThreshold, false);
		}

		// No? Find anywhere we can to move to
		else if (NavSys->GetRandomPointInNavigableRadius(PawnPoint, Config->AvoidProblemsSpreadRadius, RandomPoint, UseNavData, nullptr))
		{
			Controller->MoveToLocation(RandomPoint.Location, Config->AcceptanceThreshold, false);
		}

		else
		{
			// Do nothing, it's fine. We're stuck.
		}
	}
}

EMTaskState UMAiTask_MoveAwayFromPlayerCommand::OnPoll_Implementation(float DeltaTime)
{
	if (State != EMTaskState::Running) return State;
	if (!IsValid) return EMTaskState::Rejected;

	auto const ThisPawn = Controller->GetPawn();
	if (!ThisPawn)
	{
		return EMTaskState::Rejected;
	}
	auto const PawnPoint = ThisPawn->GetActorLocation();

	// Find the target
	auto const TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(Config->PlayerTargetKeyObject.SelectedKeyName));
	if (!TargetActor)
	{
		return EMTaskState::Rejected;
	}
	auto const TargetPoint = TargetActor->GetActorLocation();

	// Find nav system
	const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (!NavSys)
	{
		return EMTaskState::Rejected;
	}

	// Check if we got there?
	auto DistanceToTarget = (TargetPoint - PawnPoint).Size();
	if (DistanceToTarget >= Config->TargetDistance)
	{
		return EMTaskState::Resolved;
	}

	// If we're not following a path, start moving away
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
		if (Diff < Config->TargetDistance)
		{
			// The actor moved, update the destination
			UpdateMovementDestination(PawnPoint, TargetPoint, NavSys);
			return EMTaskState::Running;
		}
	}

	return State;
}

void UMAiTask_MoveAwayFromPlayerCommand::OnEnd_Implementation()
{
	RevertMoveSpeed();
}
