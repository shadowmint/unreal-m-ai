// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MAiSensors/MAiObserver.h"
#include "MAi_Rig_BaseController.generated.h"


UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAi_Rig_BaseController : public UActorComponent
{
	GENERATED_BODY()

public:
	UMAi_Rig_BaseController();

public:
	/* API ================================================================= */

	/** This should be invoked when the AI controller possess a pawn to setup the controller */
	void OnPossessPawn(APawn* Pawn);

	/** This should be invoked when the AI controller loses control of a pawn */
	void OnUnPossessPawn();

protected:
	/* API for inherited objects =========================================== */

	/** Is this controller active? */
	bool Active;

	/** This is the pawn attached to this controller */
	TWeakObjectPtr<APawn> Pawn;
	
	TWeakObjectPtr<UMAiObserver> Observer;

	/** Invoked when an event occurs */
	virtual void OnEventRaw(uint8 EventId);

	/** In this, child objects should collect all the components they need from the pawn */
	virtual bool Initialize(APawn* Pawn);

	/** In this, child objects should halt any pending tasks, etc. */
	virtual bool Shutdown();

	/** Optional; Invoked when the controller first takes control of the pawn, successfully */
	virtual void OnControllerStart();

	/** Optional; Invoked every tick on the controller */
	virtual void OnControllerTick(float DeltaTime);

	/** Register a sensor event for the controller */
	void RegisterSensorEvent(FName SensorGroup, bool IsDetected, uint8 EventId);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
