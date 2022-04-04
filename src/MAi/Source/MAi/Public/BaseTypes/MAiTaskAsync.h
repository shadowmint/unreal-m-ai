// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MAiCommand.h"
#include "MCommand.h"
#include "MTask.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MAiTaskAsync.generated.h"

struct FMAiTaskAsyncMemory : public FBTInstancedNodeMemory
{
	TWeakObjectPtr<UMAiCommand> Command;
};

/** A task that runs for multiple ticks before resolving using a UMTask to run. */
UCLASS()
class MAI_API UMAiTaskAsync : public UBTTaskNode
{
	GENERATED_BODY()

public:
	// Required properties
	
public:
	// API
	
	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	UMAiCommand* AiCommand();

	UFUNCTION(BlueprintNativeEvent, Category="MAi")
	UMTaskExecutor* AiTaskExecutor(UBehaviorTreeComponent* OwnerComp);

	/** starts this task, should return Succeeded, Failed or InProgress
	 *  (use FinishLatentTask() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** aborts this task, should return Aborted or InProgress
	 *  (use FinishLatentAbort() when returning InProgress)
	 * this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual uint16 GetSpecialMemorySize() const override;
};
