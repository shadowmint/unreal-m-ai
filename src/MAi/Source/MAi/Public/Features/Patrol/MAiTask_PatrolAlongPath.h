// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTypes/MAiTaskAsync.h"
#include "UObject/Object.h"
#include "MAiTask_PatrolAlongPath.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_PatrolAlongPath : public UMAiTaskAsync
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector PathKeyObject;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float AcceptanceThreshold = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float StepSize = 4000;

	/** Adjust the active move this often, regardless of if we're moving or not. Shorter values give smoother paths. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float StepTimer = 0.5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector StepTimerKeyFloat;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector SpreadKeyFloat;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector LastPositionKeyVector;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector StuckTimeKeyFloat;

	/** Try to un-stuck if we're in the same place for too long */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float StuckTimeout = 1.5;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float SpreadRadius = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float SpreadFrequency = 3;

	virtual UMAiCommand* AiCommand_Implementation() override;
};
