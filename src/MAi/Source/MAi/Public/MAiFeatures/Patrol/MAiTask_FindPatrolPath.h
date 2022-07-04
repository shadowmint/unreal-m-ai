// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiPatrolPath.h"
#include "MAiBaseTypes/MAiTask.h"
#include "UObject/Object.h"
#include "MAiTask_FindPatrolPath.generated.h"

UENUM(BlueprintType)
enum class EFindPatrolPathStrategy : uint8
{
	FirstMatchingTags,
	RandomMatchingTags,
	ClosestMatchingTags
};

/** Invoke this to find the right patrol path for the AI to use */
UCLASS()
class MAI_API UMAiTask_FindPatrolPath : public UMAiTask
{
	GENERATED_BODY()

private:
	/** Special logic for PathFindingStrategy::ClosestMatchingTags */
	AMAiPatrolPath* ClosestMatchingPathFrom(AAIController* Controller, TArray<AMAiPatrolPath*> Paths) const;

	/** Pick the best path using the current PathFindingStrategy */
	AMAiPatrolPath* BestMatchingPathFrom(AAIController* Controller, TArray<AMAiPatrolPath*> Paths) const;

public:
	/** Save the patrol path to this blackboard variable */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FBlackboardKeySelector PatrolPathOutputKey;

	/** The tag filter to apply */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	EFindPatrolPathStrategy PathFindingStrategy;

	/** The selection strategy */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	TArray<FName> Tags;

	virtual bool Execute_Implementation(AAIController* Controller, UBlackboardComponent* Blackboard) override;
};
