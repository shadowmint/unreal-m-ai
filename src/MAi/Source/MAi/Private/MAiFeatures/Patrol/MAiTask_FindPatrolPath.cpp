// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/Patrol/MAiTask_FindPatrolPath.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MAiFeatures/Patrol/MAiPatrolPath.h"
#include "Utils/MComponent.h"
#include "Utils/MMath.h"
#include "Utils/MWorld.h"

AMAiPatrolPath* UMAiTask_FindPatrolPath::ClosestMatchingPathFrom(AAIController* Controller, TArray<AMAiPatrolPath*> Paths) const
{
	const auto Pawn = Controller->GetPawn();
	if (!Pawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiTask_FindPatrolPath::BestMatchingPathFrom: can't pick closest when no active pawn"));
		return nullptr;
	}

	// Seek the distance to all paths and pick the closest one
	const auto Here = Pawn->GetActorLocation();
	auto ClosestPath = Paths[0];
	auto Dist = (Here - Paths[0]->GetClosestPointOnPathTo(Pawn)).SizeSquared();
	for (auto const Path : Paths)
	{
		const auto NextDist = (Here - Path->GetClosestPointOnPathTo(Pawn)).SizeSquared();
		if (NextDist < Dist)
		{
			ClosestPath = Path;
			Dist = NextDist;
		}
	}

	return ClosestPath;
}

AMAiPatrolPath* UMAiTask_FindPatrolPath::BestMatchingPathFrom(AAIController* Controller, TArray<AMAiPatrolPath*> Paths) const
{
	switch (PathFindingStrategy)
	{
	case EFindPatrolPathStrategy::FirstMatchingTags:
		return Paths[0];

	case EFindPatrolPathStrategy::RandomMatchingTags:
		return Paths[FMath::RandRange(0, Paths.Num() - 1)];

	case EFindPatrolPathStrategy::ClosestMatchingTags:
		return this->ClosestMatchingPathFrom(Controller, Paths);

	default:
		UE_LOG(LogTemp, Warning, TEXT("UMAiTask_FindPatrolPath::BestMatchingPathFrom: Unknown strategy"));
		return nullptr;
	}
}

bool UMAiTask_FindPatrolPath::Execute_Implementation(AAIController* Controller, UBlackboardComponent* Blackboard)
{
	TArray<AMAiPatrolPath*> AllPaths;

	if (FMWorld::FindActorsMatchingTags(this, Tags, AllPaths) == 0)
	{
		return false;
	}

	auto const BestMatchingPath = this->BestMatchingPathFrom(Controller, AllPaths);
	if (!BestMatchingPath)
	{
		return false;
	}

	Blackboard->SetValueAsObject(PatrolPathOutputKey.SelectedKeyName, BestMatchingPath);
	return true;
}
