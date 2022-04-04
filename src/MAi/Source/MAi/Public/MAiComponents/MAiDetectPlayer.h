// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "MAiDetectPlayer.generated.h"

UENUM(BlueprintType)
enum class EMAiDetectPlayerOnFalse : uint8
{
	SetBlackboardKeyToFalse,
	ClearBlackboardKey
};

/** Add this component to an AI actor with an AI perception to track players */
UCLASS(ClassGroup=(MAi), meta=(BlueprintSpawnableComponent))
class MAI_API UMAiDetectPlayer : public UActorComponent
{
	GENERATED_BODY()

public:
	UMAiDetectPlayer();

	TWeakObjectPtr<UAIPerceptionComponent> Perception;

	/** True if this component is active and ready */
	UPROPERTY(BlueprintReadOnly, Category="MAi")
	bool CanCurrentlySeePlayer;

	/** True if this component is active and ready */
	UPROPERTY(BlueprintReadOnly, Category="MAi")
	bool IsReady;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	int PlayerId = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FName SetPlayerVisibleBlackboardKeyBoolean;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FName SetPlayerRefBlackboardKeyObject;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	EMAiDetectPlayerOnFalse OnDetectionLost;

	/**
	 * Prevent spamming events when the monster spins or turns by making sure it can still 'see' the
	 * player for some time after the perception system technically loses sight of them.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float LostSightCoolDown = 0.5;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	bool Debug = false;
	
private:
	void ApplyLostSightOfPlayer(UBlackboardComponent* Blackboard) const;

	void ApplyGainedSightOfPlayer(APawn* CanCurrentlySee, UBlackboardComponent* Blackboard) const;

	/** Return true if the perception system can currently detect the player */
	APawn* CanCurrentlyPerceivePlayer() const;

	/** Wait cool off seconds before actually losing sight of player */
	float SecondsSinceLostPlayer = 0;

	/** If we're currently in a tick cooldown state? */
	bool LostSightCoolDownActive = false;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
