// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiCommands/MAiTask_MoveAwayFromPlayer.h"
#include "MAiCommands/MAiTask_MoveAwayFromPlayerCommand.h"

UMAiCommand* UMAiTask_MoveAwayFromPlayer::AiCommand_Implementation()
{
	auto const Instance = NewObject<UMAiTask_MoveAwayFromPlayerCommand>(this);
	Instance->Config = this;
	return Instance;
}
