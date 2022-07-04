// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiBaseTypes/MAiTask.h"
#include "MAiTask_FindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_FindPlayer : public UMAiTask
{
	GENERATED_BODY()
	
public:
	/** Save the player pawn to this blackboard variable */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector PlayerOutputKey;

	/** Get the pawn for this player id */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	int PlayerId = 0;

	virtual bool Execute_Implementation(AAIController *Controller, UBlackboardComponent *Blackboard) override;
};
