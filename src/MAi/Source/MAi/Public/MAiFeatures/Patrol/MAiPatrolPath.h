// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MAiPatrolPathDebug.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "MAiPatrolPath.generated.h"

UCLASS(BlueprintType)
class MAI_API AMAiPatrolPath : public AActor
{
	GENERATED_BODY()
	
public:
	AMAiPatrolPath(const FObjectInitializer &ObjectInitializer);
	
	/** Assign an actor to the path to preview it */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MAi")
	UMAiPatrolPathDebug *Debug;
	
	/** The spline defines the patrol path */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MAi")
	USplineComponent *Path;

public:
	// Utility API functions

	/** Return the closest point on the path to the given actor */
	FVector GetClosestPointOnPathTo(const AActor* Actor) const;
};
