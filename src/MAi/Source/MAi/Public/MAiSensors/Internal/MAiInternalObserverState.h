// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MAiSensors/MAiSensor.h"
#include "MAiInternalObserverState.generated.h"

/// Keeps track of the observer state for all sensors and observers on the level.
UCLASS()
class MAI_API AMAiInternalObserverState : public AActor
{
	GENERATED_BODY()

public:
	// Internal cache for performance
	static TWeakObjectPtr<AMAiInternalObserverState> Cached;

	static TWeakObjectPtr<AMAiInternalObserverState> GetOrCreateObserverState(UObject* WorldContextObject);

public:
	// API =================================================
	void RegisterSensor(UMAiSensor* Sensor, const FName& SensorGroup);

	void RegisterObserver(UMAiObserver* Observer);

	void RemoveSensor(UMAiSensor* Sensor);

	void RemoveObserver(UMAiObserver* Observer);

private:
	// Internal API =================================================
	TArray<TWeakObjectPtr<UMAiSensor>>& GetSensorGroup(FName Name);

	void RegisterAllSensors(UMAiObserver* Observer, FName GroupName, TArray<TWeakObjectPtr<UMAiSensor>>& SensorGroup);

private:
	// Internal State =================================================

	// Set of held sensors
	TMap<FName, TArray<TWeakObjectPtr<UMAiSensor>>> Sensors;

	// Set of held observers
	TArray<TWeakObjectPtr<UMAiObserver>> Observers;
};
