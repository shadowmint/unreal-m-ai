// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Templates/MAi_2D_MonsterRig.h"


// Sets default values for this component's properties
UMAi_2D_MonsterRig::UMAi_2D_MonsterRig()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMAi_2D_MonsterRig::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMAi_2D_MonsterRig::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMAi_2D_MonsterRig::MoveToPointNearCurrentLocation(float PatrolRadius)
{
	// Start nav, and then... like, register events or something.
}
