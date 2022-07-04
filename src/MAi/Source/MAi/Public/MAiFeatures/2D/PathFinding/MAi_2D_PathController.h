// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MAiFeatures/2D/Internal/MAi_2D_PathFinder.h"
#include "MAi_2D_PathController.generated.h"

USTRUCT(BlueprintType)
struct MAI_API FMAi_2D_PathControllerEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="MAi|2D")
	FName MovementGroup;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="MAi|2D")
	int Splines;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="MAi|2D")
	int Nodes;

	UPROPERTY()
	UMAi_2D_PathFinder* Path = nullptr;

	FMAi_2D_PathControllerEntry()
	{
		Path = nullptr;
		Nodes = 0;
		Splines = 0;
	}
	
	FMAi_2D_PathControllerEntry(FName InMovementGroup, UMAi_2D_PathFinder* InPath)
	{
		MovementGroup = InMovementGroup;
		Path = InPath;
		Splines = 0;
		Nodes = 0;
	}
};

UCLASS(BlueprintType)
class MAI_API AMAi_2D_PathController : public AActor
{
	GENERATED_BODY()

private:
	/** Internal state =================================*/
	static TWeakObjectPtr<AMAi_2D_PathController> Instance;

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="MAi|2D")
	TArray<FMAi_2D_PathControllerEntry> MovementGroups;
	
public:
	/* API ============================================= */
	UFUNCTION(BlueprintCallable, Category="MAi|2D", meta=(WorldContext="WorldContextObject"))
	static AMAi_2D_PathController* GetPathControllerForLevel(UObject* WorldContextObject);

	/** Trigger movement on some pawn */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	UMAi_2D_PathFinder* GetPathFinder(FName MovementGroup);

	/** Load a node and all the spline associated with it from the level */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void Add(UMAi_2D_PathNode* PathNode);

private:
	/* Internal ============================================= */
	FMAi_2D_PathControllerEntry &GetMovementGroup(FName MovementGroup);
};
