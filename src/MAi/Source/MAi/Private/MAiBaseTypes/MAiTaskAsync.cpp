// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiBaseTypes/MAiTaskAsync.h"

#include "AIController.h"
#include "Actors/MStdExecutor.h"

UMTaskExecutor* UMAiTaskAsync::AiTaskExecutor_Implementation(UBehaviorTreeComponent* OwnerComp)
{
	auto const Controller = Cast<AAIController>(OwnerComp->GetAIOwner());
	return AMStdExecutor::GetStdExecutor(Controller);
}

UMAiCommand* UMAiTaskAsync::AiCommand_Implementation()
{
	return nullptr;
}

EBTNodeResult::Type UMAiTaskAsync::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto const Controller = Cast<AAIController>(OwnerComp.GetAIOwner());

	auto const Cmd = AiCommand();
	if (Cmd == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiTaskAsync::Activate: nullptr back from AiCommand."));
		return EBTNodeResult::Failed;
	}

	Cmd->Initialize(Controller, &OwnerComp);
	Cmd->Update.AddLambda([&](const UMCommand* CompletedCommand)
	{
		if (CompletedCommand->State == EMTaskState::Resolved)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	});

	auto const Executor = AiTaskExecutor(&OwnerComp);
	if (Executor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiTaskAsync::Activate: nullptr back from AiTaskExecutor."));
		return EBTNodeResult::Failed;
	}

	FMAiTaskAsyncMemory* AsyncTaskMemory = GetSpecialNodeMemory<FMAiTaskAsyncMemory>(NodeMemory);
	AsyncTaskMemory->Command = Cmd;
	
	Executor->RunCommand(Cmd, this);
	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UMAiTaskAsync::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("Async task got an abort request"));
	FMAiTaskAsyncMemory* AsyncTaskMemory = GetSpecialNodeMemory<FMAiTaskAsyncMemory>(NodeMemory);
	if (AsyncTaskMemory->Command.IsValid())
	{
		auto const Executor = AiTaskExecutor(&OwnerComp);
		if (Executor == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("UMAiTaskAsync::AbortTask: nullptr back from AiTaskExecutor."));
			return EBTNodeResult::Failed;
		}

		UE_LOG(LogTemp, Warning, TEXT("Command cancelled: %s"), *AsyncTaskMemory->Command->GetName());
		AsyncTaskMemory->Command->Cancel(Executor);
		AsyncTaskMemory->Command = nullptr;
	}
	return EBTNodeResult::Aborted;
}

uint16 UMAiTaskAsync::GetSpecialMemorySize() const
{
	return sizeof(FMAiTaskAsyncMemory);
}
