// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiTask_ChaseAfterPlayer.h"
#include "NavigationSystem.h"
#include "MAiBaseTypes/MAiCommand.h"
#include "MAiTask_ChaseAfterPlayerCommand.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_ChaseAfterPlayerCommand : public UMAiCommand
{
	GENERATED_BODY()

private:
	void SetAttackSpeed() const;

	void RevertAttackSpeed() const;

	void UpdateMovementDestination(FVector PawnPoint, FVector TargetPoint, UNavigationSystemV1* NavSys);

	bool PreventPawnGettingStuck(float DeltaTime, FVector PawnPoint, FVector TargetPoint, UNavigationSystemV1* NavSys);

public:
	UPROPERTY()
	UMAiTask_ChaseAfterPlayer* Config;

	virtual auto OnStart_Implementation(UObject* Context) -> void override;

	virtual EMTaskState OnPoll_Implementation(float DeltaTime) override;

	virtual void OnEnd_Implementation() override;
};
