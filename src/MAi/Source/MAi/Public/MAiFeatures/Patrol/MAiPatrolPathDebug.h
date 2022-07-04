// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MAiPatrolPathDebug.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MAI_API UMAiPatrolPathDebug : public UActorComponent
{
	GENERATED_BODY()

public:
	UMAiPatrolPathDebug();

protected:
	/** If we have a spawned debug actor, set it here */
	TWeakObjectPtr<AActor> DebugActorInstance;

	/** The path offset */
	float PathOffset;
	
	bool SpawnDebugActor();

	void UpdateDebugActor(float DeltaTime);

	void PurgeDebugActor();

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	bool EnableDebug;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	TSubclassOf<AActor> DebugActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	FVector Offset = FVector(0, 0, 10);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	float Speed = 1;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
