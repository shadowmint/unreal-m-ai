// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAi_2D_MonsterRig.h"
#include "Components/ActorComponent.h"
#include "MAiFeatures/2D/Movement/MAi_2D_Movement.h"
#include "MAiRig/MAi_Rig_BaseController.h"
#include "MAiSensors/MAiObserver.h"
#include "MAi_2D_AggressiveBehaviour.generated.h"


enum class UMAi_2D_AggressiveBehaviourEvent : uint8
{
	DetectedPlayer,
	LostPlayer,
	ArrivedAtDestination,
	FailedToGetToDestination
};

enum class UMAi_2D_AggressiveBehaviourActivity : uint8
{
	OnPatrol,
	Idle,
	HuntingPlayer,
	ChasingPlayer
};

USTRUCT()
struct FMAi_2D_AggressiveBehaviourState
{
	GENERATED_BODY()

	UMAi_2D_AggressiveBehaviourActivity CurrentActivity = UMAi_2D_AggressiveBehaviourActivity::Idle;

	FVector PatrolDestination;

	float TimeSinceLastSawPlayer;
};

USTRUCT()
struct FMAi_2D_AggressiveBehaviourConfig
{
	GENERATED_BODY()

	/** After losing sight of the player, how long do we hunt them before giving up? */
	UPROPERTY(EditAnywhere, Category="MAI|2D")
	float HuntPlayerCooldown = 10;
	
	float PatrolRadius = 400;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAi_2D_AggressiveBehaviour : public UMAi_Rig_BaseController
{
	GENERATED_BODY()

	FMAi_2D_AggressiveBehaviourConfig Config;

	FMAi_2D_AggressiveBehaviourState State;
	
public:
	UPROPERTY()
	UMAiObserver* ObserverRef;

	UPROPERTY()
	UMAi_2D_MonsterRig* RigRef;

protected:
	/** API from base type ==================================== */
	virtual void OnEventRaw(uint8 EventId) override;

	virtual bool Initialize(APawn* Pawn) override;

	virtual bool Shutdown() override;

	virtual void OnControllerStart() override;

private:
	/** Internal API ==================================== */
	void OnEvent(UMAi_2D_AggressiveBehaviourEvent Event);
	
	void StartNewPatrolAction();
};
