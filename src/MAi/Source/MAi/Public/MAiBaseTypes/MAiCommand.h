// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MCommand.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "UObject/Object.h"
#include "MAiCommand.generated.h"

UCLASS(Abstract, BlueprintType)
class MAI_API UMAiCommand : public UMCommand
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="MAi")
	AAIController* Controller;

	UPROPERTY(BlueprintReadOnly, Category="MAi")
	UBlackboardComponent* Blackboard;

	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	void Initialize(AAIController* InController, UBehaviorTreeComponent* OwnerComp);

protected:
	/** Is this command ready to run? */
	bool IsValid;
};
