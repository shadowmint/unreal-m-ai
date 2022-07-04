// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/Patrol/MAiTask_PatrolAlongPath.h"

#include "MAiFeatures/Patrol/MAiTask_PatrolAlongPathCommand.h"

UMAiCommand* UMAiTask_PatrolAlongPath::AiCommand_Implementation()
{
	auto const Instance = NewObject<UMAiTask_PatrolAlongPathCommand>(this);
	Instance->Config = this;
	return Instance;
}
