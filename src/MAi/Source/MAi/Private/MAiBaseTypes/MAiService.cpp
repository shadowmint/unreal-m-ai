// Fill out your copyright notice in the Description page of Project Settings.

#include "MAiBaseTypes/MAiService.h"
#include "AIController.h"

void UMAiService::OnTick_Implementation(AAIController* Controller, UBlackboardComponent* Blackboard)
{
}

void UMAiService::OnStarted_Implementation(AAIController* Controller, UBlackboardComponent* Blackboard)
{
}

void UMAiService::OnStopped_Implementation(AAIController* Controller, UBlackboardComponent* Blackboard)
{
}

void UMAiService::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	auto const Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	auto const Blackboard = Controller->GetBlackboardComponent();
	OnStarted(Controller, Blackboard);
}

void UMAiService::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);
	auto const Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	auto const Blackboard = Controller->GetBlackboardComponent();
	OnStopped(Controller, Blackboard);
}

void UMAiService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto const Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
	auto const Blackboard = Controller->GetBlackboardComponent();
	OnTick(Controller, Blackboard);
}
