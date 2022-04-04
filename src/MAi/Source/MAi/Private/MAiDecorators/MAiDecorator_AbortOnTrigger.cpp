// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiDecorators/MAiDecorator_AbortOnTrigger.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

void UMAiDecorator_AbortOnTrigger::PostInitProperties()
{
	Super::PostInitProperties();
	bNotifyTick = true;
}

void UMAiDecorator_AbortOnTrigger::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto BB = OwnerComp.GetBlackboardComponent();
	const auto AbortValue = BB->GetValueAsBool(OnAbortKeyBoolean.SelectedKeyName);

	auto const BlackboardAsset = BB->GetBlackboardAsset();
	auto const KeyId = BB->GetKeyID(OnAbortKeyBoolean.SelectedKeyName);
	const FBlackboardEntry* EntryInfo = BlackboardAsset ? BlackboardAsset->GetKey(KeyId) : nullptr;
	auto const AbortValueIsSet = EntryInfo != nullptr && EntryInfo->KeyType != nullptr;

	switch (TriggerType)
	{
	case EAbortOnTriggerType::IsTrue:
		if (AbortValue) OwnerComp.RequestExecution(this);
		break;
	case EAbortOnTriggerType::IsFalse:
		if (!AbortValue) OwnerComp.RequestExecution(this);
		break;
	case EAbortOnTriggerType::IsSet:
		if (AbortValue) OwnerComp.RequestExecution(this);
		break;
	case EAbortOnTriggerType::IsNotSet:
		if (!AbortValueIsSet) OwnerComp.RequestExecution(this);
		break;
	case EAbortOnTriggerType::IsFalseOrNotSet:
		if (!AbortValue || !AbortValueIsSet)
		{
			OwnerComp.RequestExecution(this);
		}
		break;
	default: ;
	}
}
