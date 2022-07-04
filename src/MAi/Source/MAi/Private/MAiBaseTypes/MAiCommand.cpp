// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiBaseTypes/MAiCommand.h"

#include "BehaviorTree/BlackboardComponent.h"

void UMAiCommand::Initialize_Implementation(AAIController* InController, UBehaviorTreeComponent* OwnerComp)
{
	Controller = InController;
	Blackboard = Controller->GetBlackboardComponent();
	IsValid = true;
}