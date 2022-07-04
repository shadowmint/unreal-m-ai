// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAi_2D_PathNode.generated.h"

UCLASS(ClassGroup=(MAi), meta=(BlueprintSpawnableComponent))
class MAI_API UMAi_2D_PathNode : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	FName MovementGroup = "Default";

	/** Automatically register this node with the level controller */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi|2D")
	bool AutoRegister = true;
	
	// Sets default values for this component's properties
	UMAi_2D_PathNode();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FVector GetWorldLocation() const;
};
