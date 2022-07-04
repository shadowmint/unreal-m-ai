// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAi_2D_PathFinder.h"
#include "MAiFeatures/2D/Movement/MAi_2D_Movement.h"
#include "UObject/Object.h"
#include "MAi_2D_PathFollower.generated.h"

USTRUCT(BlueprintType)
struct FMAi_2D_PathFollowerState
{
	GENERATED_BODY()

	TWeakObjectPtr<UMAi_2D_Movement> Movement;

	TOptional<FMAi_2D_Path> Path;

	/** The step which is currently active and processing, if any */
	TOptional<int> CurrentStep;

	/** Have we completed the current step? */
	int PreviousStep;
};

UCLASS(BlueprintType)
class MAI_API UMAi_2D_PathFollower : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	FMAi_2D_PathFollowerState State;

public:
	/* API ================================== */

	void Initialize(UMAi_2D_Movement* Movement);

	void Stop();

	/** Check if currently active */
	bool Active() const;
	
	/** Move to a point in the world as best we can */
	bool Follow(FMAi_2D_Path Path);

	/** Update step */
	void Update(float DeltaTime);

public:
	/* Internal ================================== */
	UFUNCTION()
	void OnParentMoved(bool Arrived, FVector Location, FVector Target);
};
