// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/Patrol/MAiPatrolPathDebug.h"

#include "MAiFeatures/Patrol/MAiPatrolPath.h"

UMAiPatrolPathDebug::UMAiPatrolPathDebug()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UMAiPatrolPathDebug::SpawnDebugActor()
{
	auto const Parent = Cast<AMAiPatrolPath>(GetOwner());
	if (!Parent)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiPatrolPathDebug::SpawnDebugActor: Is only valid as a child of an MAiPatrolPath"));
		return false;
	}

	if (!DebugActor.Get())
	{
		return false;
	}

	if (DebugActorInstance.IsValid())
	{
		PathOffset = 0;
		return true;
	}

	const auto Position = GetOwner()->GetActorLocation();
	const auto Rotation = FRotator(0, 0, 0);
	auto Params = FActorSpawnParameters();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto const Spawned = GetWorld()->SpawnActor<AActor>(DebugActor, Position, Rotation, Params);
	if (!Spawned)
	{
		return false;
	}

	DebugActorInstance = Spawned;
	PathOffset = 0;
	return true;
}

void UMAiPatrolPathDebug::UpdateDebugActor(float DeltaTime)
{
	if (!DebugActorInstance.IsValid())
	{
		return;
	}

	// Step along path
	PathOffset += DeltaTime * Speed;
	PathOffset -= FMath::Floor(PathOffset); // fractional part only

	// Get offset along path
	auto const Parent = Cast<AMAiPatrolPath>(GetOwner());
	if (!Parent)
	{
		return;
	}
	const auto Position = Parent->Path->GetLocationAtTime(PathOffset, ESplineCoordinateSpace::World, true);
	const auto Direction = Parent->Path->GetDirectionAtTime(PathOffset, ESplineCoordinateSpace::World, true);
	const auto Up = Parent->Path->GetUpVectorAtTime(PathOffset, ESplineCoordinateSpace::World, true);

	// Calculate effective targets
	const auto TargetRotation = Direction.Rotation();
	const auto TargetPosition = Position + Offset;

	// Move actor
	DebugActorInstance->SetActorLocationAndRotation(TargetPosition, TargetRotation, false, nullptr, ETeleportType::ResetPhysics);
}

void UMAiPatrolPathDebug::PurgeDebugActor()
{
	if (DebugActorInstance.IsValid())
	{
		GetWorld()->RemoveActor(DebugActorInstance.Get(), true);
		DebugActorInstance = TWeakObjectPtr<AActor>();
	}
}

void UMAiPatrolPathDebug::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If the actor is missing spawn it
	if (EnableDebug && !DebugActorInstance.IsValid())
	{
		if (!SpawnDebugActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("UMAiPatrolPathDebug::TickComponent: Unable to spawn debug actor"));
			EnableDebug = false;
			return;
		}
	}

	// If the actor exists and we don't need it, purge it
	if (!EnableDebug && DebugActorInstance.IsValid())
	{
		PurgeDebugActor();
		return;
	}

	// If we're in debug mode, update the debug marker
	if (EnableDebug)
	{
		UpdateDebugActor(DeltaTime);
	}
}
