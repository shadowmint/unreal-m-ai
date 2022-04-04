// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseTypes/MAiTaskAsync.h"
#include "UObject/Object.h"
#include "MAiTask_MoveAwayFromPlayer.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_MoveAwayFromPlayer : public UMAiTaskAsync
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector PlayerTargetKeyObject;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector OldSpeedKeyFloat;

	/** If obstructed, move somewhere else this far away and try again */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float AvoidProblemsSpreadRadius;

	/** Movement acceptance threshold */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float TargetDistance = 200;
	
	/**
	 * Eg. For 2x speed while chasing set to 2.
	 * When this task aborts or ends the speed is set back to the old speed.
	 **/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float SpeedBoostWhileMovingFactor = 1;

	/** Movement acceptance threshold */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float AcceptanceThreshold = 1;

	virtual UMAiCommand* AiCommand_Implementation() override;
};
