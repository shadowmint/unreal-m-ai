// Fill out your copyright notice in the Description page of Project Settings.

#include "MAiBaseTypes/MAiTask.h"
#include "Actors/MStdExecutor.h"
#include "AIController.h"

bool UMAiTask::Execute_Implementation(AAIController *Controller, UBlackboardComponent *Blackboard)
{
	return true;
}

EBTNodeResult::Type UMAiTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	auto const Blackboard = Controller->GetBlackboardComponent();
	return Execute(Controller, Blackboard) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

EBTNodeResult::Type UMAiTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}
