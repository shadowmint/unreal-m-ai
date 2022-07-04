// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MAiObserver.h"
#include "Components/ActorComponent.h"
#include "MAiSensor.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FSensorEvent, AActor* OtherActor, bool Detected);

// This is a component you would put on a pawn to enable it to do MAi sensing. 
UCLASS(Abstract, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAiSensor : public UActorComponent
{
	GENERATED_BODY()

public:
	// Blueprint props ===============================

	// The name of the sensor groups this sensor is associated with, if any
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|Sensors")
	TArray<FName> SensorGroups;

public:
	// External API ===============================

	// Event triggered when sensor state changes
	FSensorEvent OnChange;

	/** Register this sensor with an external observer */
	void RegisterWithObserver(UMAiObserver* Observer);

protected:
	// Extension API ===============================

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void OnDetect(AActor* OtherActor);

	void OnLost(AActor* OtherActor);

private:
	// Internal API =================================

	// If the parent is a Pawn and has a controller with an observer, return it.
	TOptional<UMAiObserver*> GetParentObserver() const;

	// Initialize the state of the sensor
	void InitializeMAiSensor();
};
