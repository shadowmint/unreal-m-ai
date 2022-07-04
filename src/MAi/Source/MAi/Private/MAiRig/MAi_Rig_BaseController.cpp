// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiRig/MAi_Rig_BaseController.h"

#include "MAiSensors/MAiObserver.h"
#include "Utils/MComponent.h"

UMAi_Rig_BaseController::UMAi_Rig_BaseController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMAi_Rig_BaseController::OnPossessPawn(APawn* InPawn)
{
	if (Active && Pawn == InPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_Rig_BaseController::OnPossessPawn: Invalid attempt to possess the already owned pawn for controller"));
		return;;
	}

	// Find observer on this controller
	Observer = FMComponent::GetComponent<UMAiObserver>(GetOwner());
	if (!Observer.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_Rig_BaseController::OnPossessPawn: No UMAiObserver on Controller; it cannot be used with a controller"));
		return;
	}

	// If currently active shutdown the existing binding first
	if (Active)
	{
		Shutdown();
	}

	Pawn = InPawn;
	if (!Initialize(Pawn.Get()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAi_Rig_BaseController::OnPossessPawn: Controller initialization failed."))
		return;
	}

	Active = true;
	OnControllerStart();
}

void UMAi_Rig_BaseController::OnUnPossessPawn()
{
	Shutdown();
	Active = false;
}

void UMAi_Rig_BaseController::OnEventRaw(uint8 EventId)
{
}

bool UMAi_Rig_BaseController::Initialize(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("UMAi_Rig_BaseController::Initialize: Initialize must be implemented by child classes"))
	return false;
}

bool UMAi_Rig_BaseController::Shutdown()
{
	UE_LOG(LogTemp, Warning, TEXT("UMAi_Rig_BaseController::Shutdown: Shutdown must be implemented by child classes"))
	return false;
}

void UMAi_Rig_BaseController::OnControllerStart()
{
}

void UMAi_Rig_BaseController::OnControllerTick(float DeltaTime)
{
}

void UMAi_Rig_BaseController::RegisterSensorEvent(FName SensorGroup, bool IsDetected, uint8 EventId)
{
	UE_LOG(LogTemp, Warning, TEXT(" UMAi_Rig_BaseController::RegisterSensorEvent: Not implemented"));
}

void UMAi_Rig_BaseController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!Active) return;
	OnControllerTick(DeltaTime);
}
