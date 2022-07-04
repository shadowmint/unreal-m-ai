// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Sensors/MAiRadial2DSensor.h"


// Sets default values for this component's properties
UMAiRadial2DSensor::UMAiRadial2DSensor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMAiRadial2DSensor::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UMAiRadial2DSensor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

