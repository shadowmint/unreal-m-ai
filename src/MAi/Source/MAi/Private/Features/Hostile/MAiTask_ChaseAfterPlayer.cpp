// Fill out your copyright notice in the Description page of Project Settings.


#include "Features/Hostile/MAiTask_ChaseAfterPlayer.h"

#include "Features/Hostile/MAiTask_ChaseAfterPlayerCommand.h"

UMAiCommand* UMAiTask_ChaseAfterPlayer::AiCommand_Implementation()
{
	auto const Instance = NewObject<UMAiTask_ChaseAfterPlayerCommand>(this);
	Instance->Config = this;
	return Instance;
}
