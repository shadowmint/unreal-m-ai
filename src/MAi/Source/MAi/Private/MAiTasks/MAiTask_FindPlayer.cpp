// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiTasks/MAiTask_FindPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

bool UMAiTask_FindPlayer::Execute_Implementation(AAIController *Controller, UBlackboardComponent *Blackboard)
{
	auto const PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerId);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiTask_FindPlayer::Execute_Implementation: No such player %d"), PlayerId);
		return false;
	}

	auto const Pawn = PlayerController->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiTask_FindPlayer::Execute_Implementation: Player %d has no Pawn"), PlayerId);
		return false;
	}

	Blackboard->SetValueAsObject(PlayerOutputKey.SelectedKeyName, Pawn);
	UE_LOG(LogTemp, Display, TEXT("UMAiTask_FindPlayer::Execute_Implementation: Found %s"), *Pawn->GetName());
	
	return true;
}
