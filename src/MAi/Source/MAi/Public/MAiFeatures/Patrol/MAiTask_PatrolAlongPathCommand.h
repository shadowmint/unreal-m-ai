// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiTask_PatrolAlongPath.h"
#include "MAiBaseTypes/MAiCommand.h"
#include "UObject/Object.h"
#include "MAiTask_PatrolAlongPathCommand.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MAI_API UMAiTask_PatrolAlongPathCommand : public UMAiCommand
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	UMAiTask_PatrolAlongPath* Config;

	virtual void OnStart_Implementation(UObject* Context) override;

	virtual EMTaskState OnPoll_Implementation(float DeltaTime) override;
};
