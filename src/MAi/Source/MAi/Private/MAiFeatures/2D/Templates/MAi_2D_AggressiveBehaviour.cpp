// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Templates/MAi_2D_AggressiveBehaviour.h"

#include "MAiFeatures/2D/Templates/MAi_2D_MonsterRig.h"
#include "MAiRig/MAi_Rig_BaseRig.h"
#include "MAiSensors/MAiSensor.h"
#include "Utils/MComponent.h"

void UMAi_2D_AggressiveBehaviour::OnEvent(UMAi_2D_AggressiveBehaviourEvent Event)
{
	if (Event == UMAi_2D_AggressiveBehaviourEvent::ArrivedAtDestination)
	{
		if (State.CurrentActivity == UMAi_2D_AggressiveBehaviourActivity::OnPatrol)
		{
			StartNewPatrolAction();
			return;
		}
	}
}

void UMAi_2D_AggressiveBehaviour::OnEventRaw(uint8 EventId)
{
	OnEvent(static_cast<UMAi_2D_AggressiveBehaviourEvent>(EventId));
}

bool UMAi_2D_AggressiveBehaviour::Initialize(APawn* Pawn)
{
	RigRef = FMComponent::GetComponent<UMAi_2D_MonsterRig>(Pawn);
	if (!RigRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("No UMAi_2D_MonsterRig on Pawn; it cannot be owned by UMAi_2D_AggressiveBehaviour"));
		return false;
	}

	// Register all sensors for the pawn on the our observer
	TArray<UMAiSensor*> Sensors;
	FMComponent::GetComponents<UMAiSensor>(Pawn, Sensors);
	for (const auto Sensor : Sensors)
	{
		Sensor->RegisterWithObserver(ObserverRef);
	}

	// Register events
	RigRef->RegisterEventHandler(this);
	RegisterSensorEvent("Player", true, static_cast<uint8>(UMAi_2D_AggressiveBehaviourEvent::DetectedPlayer));

	return true;
}

bool UMAi_2D_AggressiveBehaviour::Shutdown()
{
	return true;
}

void UMAi_2D_AggressiveBehaviour::OnControllerStart()
{
	Super::OnControllerStart();
	StartNewPatrolAction();
}

void UMAi_2D_AggressiveBehaviour::StartNewPatrolAction()
{
	State.CurrentActivity = UMAi_2D_AggressiveBehaviourActivity::OnPatrol;
	RigRef->MoveToPointNearCurrentLocation(Config.PatrolRadius);
}
