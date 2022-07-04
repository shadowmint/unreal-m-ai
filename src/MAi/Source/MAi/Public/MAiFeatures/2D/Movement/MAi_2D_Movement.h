// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "MAiFeatures/2D/Internal/MAi_2D_PathFinder.h"
#include "MAi_2D_Movement.generated.h"

class UMAi_2D_PathFollower;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMAi_2D_MovementEvent, bool, Arrived, FVector, Location, FVector, Target);

DECLARE_DELEGATE_ThreeParams(FMAi_2D_MovementInternalEvent, bool Arrived, FVector Location, FVector Target);

USTRUCT(BlueprintType)
struct FMAi_2D_MovementConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	float Acceleration = 1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	float MaxVelocity = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	float StopThreshold = 0.1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	bool AbortOnHit = true;
};

USTRUCT(BlueprintType)
struct FMAi_2D_MovementState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	FVector Velocity = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	FVector Destination = FVector(0, 0, 0);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	bool Active = false;

	UPROPERTY(BlueprintReadOnly)
	UMAi_2D_PathFollower* PathFollower = nullptr;

	UPROPERTY(BlueprintReadOnly)
	USplineComponent* Spline = nullptr;

	UPROPERTY(BlueprintReadOnly)
	float SplineStartDistance = 0;

	UPROPERTY(BlueprintReadOnly)
	float SplineEndDistance = 0;

	/** The point on the spline should be StartDistance + TravelDistance; so travel distance may be negative */
	UPROPERTY(BlueprintReadOnly)
	float SplineTraveledDistance = 0;

	/** Do we travel on the +ve for -ve spline direction for this movement? */
	UPROPERTY(BlueprintReadOnly)
	float SplineTravelDirection = 1;

	/** Quick cache; total spline length */
	float SplineLength = 0;

	/** Quick cache; distance we need to travel; eg. 0.1 -> 0.9 on a closed path is 0.2 */
	float SplineDistanceToTravel;

	/** How far have we moved since we started moving? */
	float LinearMovedDistance;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAi_2D_Movement : public UActorComponent
{
	GENERATED_BODY()

public:
	UMAi_2D_Movement();

	UPROPERTY(BlueprintAssignable, Category="MAi|2D")
	FMAi_2D_MovementEvent OnMoved;

	FMAi_2D_MovementInternalEvent OnMovedInternal;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	FMAi_2D_MovementState State;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	FMAi_2D_MovementConfig Config;

protected:
	virtual void BeginPlay() override;

public:
	/* API ===================== */

	/** Set a destination to move towards */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void NavigateTo(FVector InDestination, FName MovementGroup = "Default");

	/**
	 * Set a destination to move towards and does a linear move towards it.
	 * Stops any current movement action.
	 */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void MoveTo(FVector InDestination);

	/**
	 * Move along a section of spline, instead of to specific destination point. 
	 * Stops any current movement action.
	 */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void MoveAlongSpline(USplineComponent* AlongSpline, float FromDistance, float ToDistance);

	/**
	 * Move along a specific determined path .
	 * Stops any current movement action.
	 */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void MoveAlongPath(FMAi_2D_Path Path);

	/** Stop moving right now */
	UFUNCTION(BlueprintCallable, Category="MAi|2D")
	void Stop();

public:
	/* Internal ===================== */
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Stop moving and trigger broadcast events */
	void StopWithReason(bool Arrived, bool Internal, bool KeepVelocity);

	/** Set a destination to move towards and does a linear move towards it */
	void MoveToInternal(FVector InDestination);

	/** Move along a section of spline, instead of to specific destination point */
	void MoveAlongSplineInternal(USplineComponent* AlongSpline, float FromDistance, float ToDistance);

	/** Move along a specific determined path */
	void MoveAlongPathInternal(FMAi_2D_Path Path);

private:
	/* Internal ===================== */

	/** Debug path for debugging purposes */
	static void DebugLogPath(FMAi_2D_Path Path);

	/** Process linear movement */
	void LinearMovementUpdate(float DeltaTime);

	/** Process spline movement */
	void SplineMovementUpdate(float DeltaTime);

	/** See implementation; determine which 'way' we walk along the spline*/
	static float DetermineSplineTravelDirection(USplineComponent* AlongSpline, float Start, float End);

	/** Determine what the shortest distance between A and B is, including 'closed spline' case */
	float ShortestTravelDistance(USplineComponent* AlongSpline, float Start, float End);

	/** Given a value like 10000, convert it into a valid spline distance; on an open spline it is max, on a closed it is v % length */
	static float ToSplineLengthValue(const USplineComponent* Spline, float Distance);
};
