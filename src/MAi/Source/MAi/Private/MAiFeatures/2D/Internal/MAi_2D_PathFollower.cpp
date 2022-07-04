// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Internal/MAi_2D_PathFollower.h"

#define UMAi_2D_PathFollower_DEBUG 1

void UMAi_2D_PathFollower::Initialize(UMAi_2D_Movement* Movement)
{
	State.Movement = Movement;
	State.Movement->OnMovedInternal.BindLambda([this](bool Arrived, FVector Location, FVector Dest) { OnParentMoved(Arrived, Location, Dest); });
}

void UMAi_2D_PathFollower::Stop()
{
	State.Path.Reset();
	State.CurrentStep.Reset();
}

bool UMAi_2D_PathFollower::Active() const
{
	if (!State.Movement.IsValid()) return false;
	if (!State.Path) return false;
	return true;
}

bool UMAi_2D_PathFollower::Follow(FMAi_2D_Path Path)
{
	if (!State.Movement.IsValid())
	{
		State.Path = TOptional<FMAi_2D_Path>();
		State.CurrentStep.Reset();
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_MovementPath::MoveTo: Invalid attempt to move when no active Movement component"));
		return false;
	}

	if (!Path.IsValidPath)
	{
		State.Path = TOptional<FMAi_2D_Path>();
		State.CurrentStep.Reset();
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_MovementPath::MoveTo: Invalid attempt to move over an invalid path"));
		return false;
	}

	if (Path.Steps.Num() >0)
	{
		UE_LOG(LogTemp, Display, TEXT("UMAi_2D_PathFollower::Follow: First destination %s"), *Path.Steps[0].Destination.ToString());
	}
	
	State.Path = Path;
	State.CurrentStep.Reset();
	State.PreviousStep = -1;
#if UMAi_2D_PathFollower_DEBUG
	UE_LOG(LogTemp, Display, TEXT("UMAi_2D_PathFollower::Follow: Started path with %d steps"), State.Path->Steps.Num());
#endif
	return true;
}

void UMAi_2D_PathFollower::Update(float DeltaTime)
{
	if (!Active()) return;

	// Do we have an active step? If so, wait for arrival event
	if (State.CurrentStep.IsSet())
	{
		return;
	}

	// If not, we need to start a new step
	const auto NextStepOffset = State.PreviousStep + 1;

	// No more steps? If so, we're done
	if (NextStepOffset >= State.Path->Steps.Num())
	{
		State.Path = TOptional<FMAi_2D_Path>();
		State.CurrentStep.Reset();
		State.Movement->StopWithReason(true, false, false);
		return;
	}

	// If we got here, start the new start
#if UMAi_2D_PathFollower_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_PathFollower::Update: Moving to next step in path: %d/%d"), NextStepOffset+1, State.Path->Steps.Num());
#endif
	const auto NextStep = State.Path->Steps[NextStepOffset];
	State.CurrentStep = NextStepOffset;
	State.PreviousStep = NextStepOffset;

	// Linear steps we just trigger
	if (NextStep.Type == FMAi_2D_PathStepType::Linear)
	{
		State.Movement->MoveToInternal(NextStep.Destination);
	}

	// Spline steps we trigger explicitly
	else if (NextStep.Type == FMAi_2D_PathStepType::Spline)
	{
		State.Movement->MoveAlongSplineInternal(NextStep.Spline, NextStep.SourceDistance, NextStep.DestinationDistance);
	}

	// Otherwise, warn and skip
	else
	{
		State.CurrentStep.Reset();
		UE_LOG(LogTemp, Warning, TEXT("Invalid path step type; skipping step"));
	}
}

void UMAi_2D_PathFollower::OnParentMoved(bool Arrived, FVector Location, FVector Target)
{
	// Something went wrong...
	if (!Arrived)
	{
		State.Path = TOptional<FMAi_2D_Path>();
		State.CurrentStep.Reset();
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_MovementPath::MoveTo: Failed to arrive at destination"));
		return;
	}

	// Otherwise clear the current step; when we next update we'll pick the next one.
	State.CurrentStep.Reset();
}
