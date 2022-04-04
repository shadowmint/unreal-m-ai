// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTypes/MAiTaskAsync.h"
#include "UObject/Object.h"
#include "MAiTask_OrbitPlayer.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_OrbitPlayer : public UMAiTaskAsync
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector TargetObjectKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector RotationStateKey;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float Radius;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float Speed;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float AcceptanceThreshold = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float TeleportIfOverThreshold = 500;
	
	virtual UMAiCommand* AiCommand_Implementation() override;
};
