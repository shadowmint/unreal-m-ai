// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/PathFinding/MAi_2D_PathNode.h"
#include "MAiFeatures/2D/PathFinding/MAi_2D_PathController.h"
#include "Components/ActorComponent.h"

// Sets default values for this component's properties
UMAi_2D_PathNode::UMAi_2D_PathNode()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMAi_2D_PathNode::BeginPlay()
{
	Super::BeginPlay();
	if (AutoRegister)
	{
		auto const Controller = AMAi_2D_PathController::GetPathControllerForLevel(GetOwner());
		if (!Controller)
		{
			UE_LOG(LogTemp, Warning, TEXT("UMAi_2D_PathNode::BeginPlay: No AMAi_2D_PathController on level to bind to"));
			return;
		}

		Controller->Add(this);
	}
}


// Called every frame
void UMAi_2D_PathNode::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FVector UMAi_2D_PathNode::GetWorldLocation() const
{
	return GetComponentLocation();
}
