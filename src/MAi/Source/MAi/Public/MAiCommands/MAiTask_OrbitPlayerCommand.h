// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiTask_OrbitPlayer.h"
#include "MCommand.h"
#include "BaseTypes/MAiCommand.h"
#include "UObject/Object.h"
#include "MAiTask_OrbitPlayerCommand.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MAI_API UMAiTask_OrbitPlayerCommand : public UMAiCommand
{
	GENERATED_BODY()

	float Elapsed;
	
public:
	UPROPERTY()
	UMAiTask_OrbitPlayer* Config;

	virtual void OnStart_Implementation(UObject* Context) override;

	virtual EMTaskState OnPoll_Implementation(float DeltaTime) override;
};
