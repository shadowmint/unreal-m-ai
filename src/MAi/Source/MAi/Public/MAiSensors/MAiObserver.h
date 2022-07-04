// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MAiObserver.generated.h"

class UMAiSensor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FObserverEvent, FName, SensorGroup, AActor *, OtherActor, bool, Detected);

struct MAI_API FMAiObserved
{
	TWeakObjectPtr<UMAiSensor> Sensor;
	TOptional<FName> SensorGroup;
	FDelegateHandle Binding;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAiObserver : public UActorComponent
{
	GENERATED_BODY()

public:
	// The set of the sensor groups this observer is subscribed to, if any
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|Sensors")
	TArray<FName> SensorGroups;

	UPROPERTY(BlueprintAssignable, Category="MAi|Sensors")
	FObserverEvent OnDetection;
	
protected:
	// Sensors we're tracking
	TArray<FMAiObserved> Sensors;

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void RegisterSensor(UMAiSensor* Sensor, TOptional<FName> InSensorGroup);

	void SensorEvent(TOptional<FName> SensorGroup, AActor* OtherActor, bool Detected);

	void RemoveSensor(UMAiSensor* Sensor, FName SensorGroup);

	void ClearSensors();
};
