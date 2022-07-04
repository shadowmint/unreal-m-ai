// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAi_2D_SplineGraph.h"
#include "UObject/Object.h"
#include "MAi_2D_PathFinder.generated.h"

UENUM(BlueprintType)
enum class FMAi_2D_PathStepType : uint8
{
	Linear,
	Spline
};

USTRUCT(BlueprintType)
struct MAI_API FMAi_2D_PathStep
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	FMAi_2D_PathStepType Type;

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	USplineComponent* Spline = nullptr;

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	FVector Source = FVector::Zero();

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	FVector Destination = FVector::Zero();

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	float SourceDistance = 0;

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	float DestinationDistance = 0;

	FMAi_2D_PathStep()
	{
		Type = FMAi_2D_PathStepType::Linear;
	}
	
	FMAi_2D_PathStep(FVector From, FVector To)
	{
		Type = FMAi_2D_PathStepType::Linear;
		Source = From;
		Destination = To;
	}

	FMAi_2D_PathStep(USplineComponent *InSpline, float From, float To)
	{
		Type = FMAi_2D_PathStepType::Spline;
		Spline = InSpline;
		SourceDistance = From;
		DestinationDistance = To;
	}
};

USTRUCT(BlueprintType)
struct MAI_API FMAi_2D_Path
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	bool IsValidPath = true;

	UPROPERTY(BlueprintReadOnly, Category="MAi|2D")
	TArray<FMAi_2D_PathStep> Steps;
};

UCLASS(BlueprintType)
class MAI_API UMAi_2D_PathFinder : public UObject
{
	GENERATED_BODY()

private:
	FMAi_2D_SplineGraph Graph;

public:
	void Initialize();

	/** Find a path from a point to another point */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	FMAi_2D_Path FindPath(FVector FromPoint, FVector ToPoint);

	/** Pack the path */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void Pack();

	/** Load splines and nodes from the given target */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void DiscoverSplinesAndNodes(AActor* Actor);

	/** Create a new path manager */
	UFUNCTION(BlueprintCallable, Category="MAi|2D", meta=(WorldContext="WorldContextObject"))
	static UMAi_2D_PathFinder* MakePathFinder(UObject* WorldContextObject);

	int NumNodes() const;

	int NumSplines() const;
};
