// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiRig/MAi_Rig_BaseRig.h"
#include "MAi_2D_MonsterRig.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAi_2D_MonsterRig : public UMAi_Rig_BaseRig
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMAi_2D_MonsterRig();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MoveToPointNearCurrentLocation(float PatrolRadius);
};
