// Fill out your copyright notice in the Description page of Project Settings.


#include "Features/Patrol/MAiPatrolPath.h"

AMAiPatrolPath::AMAiPatrolPath(const FObjectInitializer& ObjectInitializer)
{
	Debug = CreateDefaultSubobject<UMAiPatrolPathDebug>(TEXT("Debug"));
	Path = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	RootComponent = Path;
}

FVector AMAiPatrolPath::GetClosestPointOnPathTo(const AActor* Actor) const
{
	if (!Actor) return FVector(0, 0, 0);
	auto const Point = Actor->GetActorLocation();
	return Path->FindLocationClosestToWorldLocation(Point, ESplineCoordinateSpace::World);
}
