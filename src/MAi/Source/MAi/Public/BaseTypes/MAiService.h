// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BehaviorTree/BTService.h"
#include "MAiService.generated.h"

/** A service is run every tick and is used as a decorator on nodes */
UCLASS(Abstract, BlueprintType)
class MAI_API UMAiService : public UBTService
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	void OnTick(AAIController* Controller, UBlackboardComponent* Blackboard);

	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	void OnStarted(AAIController* Controller, UBlackboardComponent* Blackboard);

	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	void OnStopped(AAIController* Controller, UBlackboardComponent* Blackboard);

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
