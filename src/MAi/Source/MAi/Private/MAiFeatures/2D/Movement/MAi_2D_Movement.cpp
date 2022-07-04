// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Movement/MAi_2D_Movement.h"

#include "MAiFeatures/2D/Internal/MAi_2D_PathFollower.h"
#include "MAiFeatures/2D/PathFinding/MAi_2D_PathController.h"
#include "Materials/MaterialExpressionActorPositionWS.h"

#define UMAi_2D_Movement_DEBUG 0

// Sets default values for this component's properties
UMAi_2D_Movement::UMAi_2D_Movement()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UMAi_2D_Movement::BeginPlay()
{
	Super::BeginPlay();
	State.PathFollower = NewObject<UMAi_2D_PathFollower>();
	State.PathFollower->Initialize(this);
}

// Called every frame
void UMAi_2D_Movement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!State.Active) return;

	State.PathFollower->Update(DeltaTime);

	if (State.Spline)
	{
		SplineMovementUpdate(DeltaTime);
	}
	else
	{
		LinearMovementUpdate(DeltaTime);
	}
}


void UMAi_2D_Movement::DebugLogPath(FMAi_2D_Path Path)
{
	UE_LOG(LogTemp, Display, TEXT("UMAi_2D_Movement::DebugLogPath: %d steps in path"), Path.Steps.Num());
	for (int i = 0; i < Path.Steps.Num(); i++)
	{
		auto const Step = Path.Steps[i];
		if (Step.Type == FMAi_2D_PathStepType::Linear)
		{
			UE_LOG(LogTemp, Display, TEXT("UMAi_2D_Movement::DebugLogPath: - step %d: Linear: %s -> %s"),
			       i + 1,
			       *Step.Source.ToString(),
			       *Step.Destination.ToString());
		}
		else if (Step.Type == FMAi_2D_PathStepType::Spline)
		{
			UE_LOG(LogTemp, Display, TEXT("UMAi_2D_Movement::DebugLogPath: - step %d: Spline: %s: %f -> %f"),
			       i + 1,
			       *Step.Spline->GetName(),
			       Step.SourceDistance,
			       Step.DestinationDistance);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("UMAi_2D_Movement::DebugLogPath: - step %d: Unknown step type"), i + 1);
		}
	}
}

void UMAi_2D_Movement::NavigateTo(FVector Destination, FName MovementGroup)
{
	auto Controller = AMAi_2D_PathController::GetPathControllerForLevel(GetOwner());
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::NavigateTo: No level controller to use for navigation"));
		return;
	}

	auto PathFinder = Controller->GetPathFinder(MovementGroup);
	if (!PathFinder)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::NavigateTo: No movement group found matching: %s"), *MovementGroup.ToString());
		return;
	}

	auto Here = GetOwner()->GetActorLocation();
	auto Path = PathFinder->FindPath(Here, Destination);
	if (!Path.IsValidPath)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::NavigateTo: No viable path to destination"));
		return;
	}

#if UMAi_2D_Movement_DEBUG
	DebugLogPath(Path);
#endif
	MoveAlongPath(Path);
}

void UMAi_2D_Movement::MoveTo(FVector InDestination)
{
	StopWithReason(false, false, true);
	MoveToInternal(InDestination);
}

void UMAi_2D_Movement::MoveToInternal(FVector InDestination)
{
	State.LinearMovedDistance = 0;
	State.Destination = InDestination;
	State.Active = true;
}

void UMAi_2D_Movement::MoveAlongSpline(USplineComponent* AlongSpline, float FromDistance, float ToDistance)
{
	StopWithReason(false, false, true);
	MoveAlongSplineInternal(AlongSpline, FromDistance, ToDistance);
}

void UMAi_2D_Movement::MoveAlongSplineInternal(USplineComponent* AlongSpline, float FromDistance, float ToDistance)
{
	if (!AlongSpline)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::MoveAlongSpline: Invalid spline (null)"))
		return;
	}

	State.SplineLength = AlongSpline->GetSplineLength();
	if (State.SplineLength <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::MoveAlongSpline: Invalid spline (zero length)"))
		return;
	}

	State.Active = true;
	State.Spline = AlongSpline;
	State.SplineStartDistance = ToSplineLengthValue(AlongSpline, FromDistance);
	State.SplineEndDistance = ToSplineLengthValue(AlongSpline, ToDistance);
	State.SplineTraveledDistance = 0;
	State.SplineTravelDirection = DetermineSplineTravelDirection(AlongSpline, State.SplineStartDistance, State.SplineEndDistance);
	State.SplineDistanceToTravel = ShortestTravelDistance(AlongSpline, State.SplineStartDistance, State.SplineEndDistance);
}

void UMAi_2D_Movement::MoveAlongPath(FMAi_2D_Path Path)
{
	StopWithReason(false, false, true);
	MoveAlongPathInternal(Path);
}

void UMAi_2D_Movement::MoveAlongPathInternal(FMAi_2D_Path Path)
{
	State.Active = true;
	State.PathFollower->Follow(Path);
}

void UMAi_2D_Movement::Stop()
{
	StopWithReason(false, false, false);
}

void UMAi_2D_Movement::StopWithReason(bool Arrived, bool Internal, bool KeepVelocity)
{
	auto const Dest = State.Destination;
	auto const Here = GetOwner()->GetActorLocation();

	// If we're managed, only do an internal broadcast
	if (Internal)
	{
		OnMovedInternal.Execute(Arrived, Here, Dest);
		return;
	}

	if (State.Active)
	{
		State.Active = false;

		// Keep the velocity if this is a new set of orders.
		// For example, if we need to stop and then immediately do something else.
		// eg. Redirect to new chase target by Move -> Move, instead of Move -> Stop -> Move.
		if (!KeepVelocity)
		{
			State.Velocity = FVector::Zero();
		}

		State.PathFollower->Stop();
		OnMoved.Broadcast(Arrived, Here, Dest);
	}
}

void UMAi_2D_Movement::LinearMovementUpdate(float DeltaTime)
{
	FVector ActorPosition = GetOwner()->GetActorLocation();
	FVector DirectionToTarget = State.Destination - ActorPosition;

	// Update velocity if we're not there yet.
	if (DirectionToTarget.Length() >= Config.StopThreshold)
	{
		if (!DirectionToTarget.Normalize())
		{
			UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_Movement::LinearMovementUpdate: Failed to normalize direction vector, halting"));
			State.Velocity = FVector::Zero();
		}
		else
		{
			// Add acceleration from this frame
			auto NextVelocity = State.Velocity;
			NextVelocity += DirectionToTarget * Config.Acceleration * DeltaTime;

			// Clamp value
			if (NextVelocity.Length() > Config.MaxVelocity)
			{
				NextVelocity.Normalize();
				NextVelocity = NextVelocity * Config.MaxVelocity;
			}

			// Update
			State.Velocity = NextVelocity;
		}
	}

	// If we got here, we're not there yet, move.
	auto Step = State.Velocity * DeltaTime;
	auto NextPos = GetOwner()->GetActorLocation() + Step;
	FVector DirectionToTargetAfterMove = State.Destination - NextPos;
	DirectionToTargetAfterMove.Normalize();
	if ((DirectionToTarget - DirectionToTargetAfterMove).Length() > KINDA_SMALL_NUMBER)
	{
		if (State.LinearMovedDistance > Config.StopThreshold)
		{
			UE_LOG(LogTemp, Warning, TEXT("Direction changed from %s -> %s after movement %f, jump!"), *DirectionToTarget.ToString(), *DirectionToTargetAfterMove.ToString(), State.LinearMovedDistance);
			NextPos = State.Destination;
			State.LinearMovedDistance = 0;
		}
		else
		{
			// Patch velocity
			State.Velocity = DirectionToTargetAfterMove * State.Velocity.Length();
		}
	}
	FHitResult hit;
	if (!GetOwner()->SetActorLocation(NextPos, true, &hit, ETeleportType::None))
	{
		if (Config.AbortOnHit)
		{
			Stop();
		}
		else
		{
			// TODO: start movement cooloff	
		}
	}
	else
	{
		State.LinearMovedDistance += Step.Length();
	}

	// Check if we arrived
	auto Delta = State.Destination - NextPos;
	if (Delta.Length() < Config.StopThreshold)
	{
		StopWithReason(true, State.PathFollower->Active(), true);
	}
}

void UMAi_2D_Movement::SplineMovementUpdate(float DeltaTime)
{
	// Update velocity
	if (State.Velocity.Length() < Config.MaxVelocity)
	{
		auto NextVelocity = State.Velocity.Length();
		auto const VelocityChange = Config.Acceleration * DeltaTime;

		// Add acceleration from this frame
		NextVelocity += VelocityChange;

		// Clamp value
		if (NextVelocity > Config.MaxVelocity)
		{
			NextVelocity = Config.MaxVelocity;
		}

		// Update
		State.Velocity = FVector(NextVelocity, 0, 0);
	}

	// Move
	auto const DistanceTravel = State.Velocity.Length() * DeltaTime * State.SplineTravelDirection;
	State.SplineTraveledDistance += DistanceTravel;
	auto DistanceDownSpline = State.SplineStartDistance + State.SplineTraveledDistance;

	// Wrap around edges
	if (State.Spline->IsClosedLoop())
	{
		while (DistanceDownSpline < 0)
		{
			DistanceDownSpline += State.SplineLength;
		}
		while (DistanceDownSpline > State.SplineLength)
		{
			DistanceDownSpline -= State.SplineLength;
		}
	}

	// Apply movement
	auto const NextPos = State.Spline->GetLocationAtDistanceAlongSpline(DistanceDownSpline, ESplineCoordinateSpace::World);
	FHitResult hit;
	if (!GetOwner()->SetActorLocation(NextPos, true, &hit, ETeleportType::None))
	{
		if (Config.AbortOnHit)
		{
			Stop();
		}
		else
		{
			// TODO: start movement cooloff	
		}
	}

	// Check if we arrived
	const auto TotalDistanceTraveled = FMath::Abs(State.SplineTraveledDistance);
	if (TotalDistanceTraveled > State.SplineDistanceToTravel)
	{
		State.Spline = nullptr;
		StopWithReason(true, State.PathFollower->Active(), true);
	}
}

float UMAi_2D_Movement::DetermineSplineTravelDirection(USplineComponent* AlongSpline, float Start, float End)
{
	// In the trivial case of an open spline we always move up or down it
	if (!AlongSpline->IsClosedLoop())
	{
		return End > Start ? 1 : -1;
	}

	// In the case of a closed loop, we have to pick the shortest path from the source to the destination.
	// That means we calculate the distance along both directions down the spline, and take the shortest path.
	auto const Length = AlongSpline->GetSplineLength();

	// Normal direction around spline
	if (End >= Start)
	{
		const auto PositiveDist = End - Start;
		const auto NegativeDist = Start + (Length - End);
		return PositiveDist <= NegativeDist ? 1 : -1;
	}

	// Going backwards along spline
	const auto NegativeDist = Start - End;
	const auto PositiveDist = End + (Length - Start);
	return PositiveDist <= NegativeDist ? 1 : -1;
}

float UMAi_2D_Movement::ShortestTravelDistance(USplineComponent* AlongSpline, float Start, float End)
{
	// In the trivial case of an open spline we always move up or down it
	if (!AlongSpline->IsClosedLoop())
	{
		return FMath::Abs(End - Start);
	}

	// In the case of a closed loop, we have to pick the shortest path from the source to the destination.
	// That means we calculate the distance along both directions down the spline, and take the shortest path.
	auto const Length = AlongSpline->GetSplineLength();

	// Normal direction around spline
	if (End >= Start)
	{
		const auto PositiveDist = End - Start;
		const auto NegativeDist = Start + (Length - End);
		return FMath::Min(PositiveDist, NegativeDist);
	}

	// Going backwards along spline
	const auto NegativeDist = Start - End;
	const auto PositiveDist = End + (Length - Start);
	return FMath::Min(PositiveDist, NegativeDist);
}

float UMAi_2D_Movement::ToSplineLengthValue(const USplineComponent* Spline, float Distance)
{
	auto const Length = Spline->GetSplineLength();
	if (Length == 0) return 0;

	// For an open spline, the bounds are the length of spline, that's it.
	if (!Spline->IsClosedLoop())
	{
		if (Distance < 0) return 0;
		if (Distance > Length) return Length;
		return Distance;
	}

	// For a closed spline, wrap around the length and 0
	if (Distance < 0)
	{
		auto Val = Distance;
		while (Val < 0)
		{
			Val += Length;
		}
		return Val;
	}
	if (Distance > Length)
	{
		auto Val = Distance;
		while (Val > Length)
		{
			Val -= Length;
		}
		return Val;
	}
	return Distance;
}
