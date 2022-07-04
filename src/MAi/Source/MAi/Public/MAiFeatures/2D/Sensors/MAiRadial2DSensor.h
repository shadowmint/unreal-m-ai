// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MAiSensors/MAiSensor.h"
#include "MAiRadial2DSensor.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAiRadial2DSensor : public UMAiSensor
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMAiRadial2DSensor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
