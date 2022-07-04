// Fill out your copyright notice in the Description page of Project Settings.

#include "MAiFeatures/2D/PathFinding/MAi_2D_PathController.h"
#include "Kismet/GameplayStatics.h"

TWeakObjectPtr<AMAi_2D_PathController> AMAi_2D_PathController::Instance;

AMAi_2D_PathController* AMAi_2D_PathController::GetPathControllerForLevel(UObject* WorldContextObject)
{
	if (!Instance.IsValid())
	{
		const auto Actor = Cast<AMAi_2D_PathController>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
		if (!Actor) return nullptr;
		Instance = Actor;
	}
	return Instance.Get();
}

UMAi_2D_PathFinder* AMAi_2D_PathController::GetPathFinder(FName MovementGroup)
{
	for (auto& Group : MovementGroups)
	{
		if (Group.MovementGroup == MovementGroup)
		{
			Group.Path->Pack();
			return Group.Path;
		}
	}
	return nullptr;
}

void AMAi_2D_PathController::Add(UMAi_2D_PathNode* PathNode)
{
	if (!PathNode) return;
	auto& Group = GetMovementGroup(PathNode->MovementGroup);
	Group.Path->DiscoverSplinesAndNodes(PathNode->GetOwner());
	Group.Nodes = Group.Path->NumNodes();
	Group.Splines = Group.Path->NumSplines();
}

FMAi_2D_PathControllerEntry& AMAi_2D_PathController::GetMovementGroup(FName MovementGroup)
{
	for (auto& Group : MovementGroups)
	{
		if (Group.MovementGroup == MovementGroup)
		{
			return Group;
		}
	}
	const auto Path = UMAi_2D_PathFinder::MakePathFinder(this);
	MovementGroups.Push(FMAi_2D_PathControllerEntry(MovementGroup, Path));
	return MovementGroups[MovementGroups.Num() - 1];
}
