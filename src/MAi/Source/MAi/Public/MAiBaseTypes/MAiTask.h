// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../../../../MTasks/Source/MTasks/Public/MTask.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "Tasks/AITask.h"
#include "UObject/Object.h"
#include "MAiTask.generated.h"

/** A task that runs immediately and then passes or fails; eg. calculations */
UCLASS(Abstract, BlueprintType)
class MAI_API UMAiTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	/** Return true on success */
	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	bool Execute(AAIController *Controller, UBlackboardComponent *Blackboard);

	/** starts this task, should return Succeeded, Failed or InProgress
	 *  (use FinishLatentTask() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	/** aborts this task, should return Aborted or InProgress
	 *  (use FinishLatentAbort() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
