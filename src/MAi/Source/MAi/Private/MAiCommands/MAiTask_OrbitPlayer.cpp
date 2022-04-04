// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiCommands/MAiTask_OrbitPlayer.h"

#include "MAiCommands/MAiTask_OrbitPlayerCommand.h"

UMAiCommand* UMAiTask_OrbitPlayer::AiCommand_Implementation()
{
	auto const Instance = NewObject<UMAiTask_OrbitPlayerCommand>(this);
	Instance->Config = this;
	return Instance;
}
