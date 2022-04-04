// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiCommands/MAiTask_OrbitPlayerCommand.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Utils/MStaticArray.h"

void UMAiTask_OrbitPlayerCommand::OnStart_Implementation(UObject* Context)
{
}

EMTaskState UMAiTask_OrbitPlayerCommand::OnPoll_Implementation(float DeltaTime)
{
	if (State != EMTaskState::Running) return State;
	if (!IsValid) return EMTaskState::Rejected;

	Elapsed += DeltaTime;
	if (Elapsed > 30)
	{
		// Restart tasks at least every one seconds to prevent 'stuck' behaviour.
		return EMTaskState::Resolved;
	}

	// If we're busy moving, then resolve that first
	if (Controller->IsFollowingAPath())
	{
		return State;
	}

	auto Target = Cast<AActor>(Blackboard->GetValueAsObject(Config->TargetObjectKey.SelectedKeyName));
	if (!Target)
	{
		return EMTaskState::Rejected;
	}

	auto const ThisPawn = Controller->GetPawn();
	if (!ThisPawn)
	{
		return EMTaskState::Rejected;
	}

	auto Offset = Blackboard->GetValueAsFloat(Config->RotationStateKey.SelectedKeyName);
	auto Radius = Config->Radius;
	auto Speed = Config->Speed;

	Offset += Speed * DeltaTime;
	if (Offset > PI * 2)
	{
		Blackboard->SetValueAsFloat(Config->RotationStateKey.SelectedKeyName, 0);
		return EMTaskState::Resolved;
	}

	auto OffsetVector = FVector(FMath::Sin(Offset), FMath::Cos(Offset), 0) * (Radius * 2 + FMath::Sin(Offset) * Radius);
	auto ActualTargetPoint = Target->GetActorLocation() + OffsetVector;

	auto Here = ThisPawn->GetActorLocation();
	auto Distance = (Here - ActualTargetPoint).Size();
	if (Config->TeleportIfOverThreshold < 0 || Distance < Config->TeleportIfOverThreshold)
	{
		Blackboard->SetValueAsFloat(Config->RotationStateKey.SelectedKeyName, Offset);
	}
	else
	{
		ThisPawn->SetActorLocation(ActualTargetPoint, true);
	}

	auto Result = Controller->MoveToLocation(ActualTargetPoint, Config->AcceptanceThreshold, false);
	if (Result == EPathFollowingRequestResult::Failed)
	{
		return EMTaskState::Rejected;
	}

	return State;
}
