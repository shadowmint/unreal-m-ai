// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiTask_MoveAwayFromPlayer.h"
#include "NavigationSystem.h"
#include "MAiBaseTypes/MAiCommand.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/Object.h"
#include "MAiTask_MoveAwayFromPlayerCommand.generated.h"

/**
 * 
 */
UCLASS()
class MAI_API UMAiTask_MoveAwayFromPlayerCommand : public UMAiCommand
{
	GENERATED_BODY()

private:
	void SetMoveSpeed() const;

	void RevertMoveSpeed() const;

	void UpdateMovementDestination(FVector PawnPoint, FVector ActorPoint, UNavigationSystemV1* NavSys);

public:
	UPROPERTY()
	UMAiTask_MoveAwayFromPlayer* Config;

	virtual auto OnStart_Implementation(UObject* Context) -> void override;

	virtual EMTaskState OnPoll_Implementation(float DeltaTime) override;

	virtual void OnEnd_Implementation() override;
};
