// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "UObject/Object.h"
#include "MAiDecorator_AbortOnTrigger.generated.h"

UENUM(BlueprintType)
enum class EAbortOnTriggerType : uint8
{
	IsTrue,
	IsFalse,
	IsSet,
	IsNotSet,
	IsFalseOrNotSet
};

UCLASS()
class MAI_API UMAiDecorator_AbortOnTrigger : public UBTDecorator
{
	GENERATED_BODY()

	UPROPERTY(Category=Decorator, EditAnywhere)
	FBlackboardKeySelector OnAbortKeyBoolean;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="MAi")
	EAbortOnTriggerType TriggerType;

protected:
	virtual void PostInitProperties() override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
