// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiSensors/MAiSensor.h"

#include "MAiSensors/Internal/MAiInternalObserverState.h"

TOptional<UMAiObserver*> UMAiSensor::GetParentObserver() const
{
	const auto AsPawn = Cast<APawn>(GetOwner());
	if (!AsPawn) return TOptional<UMAiObserver*>();

	const auto Controller = AsPawn->GetController();
	if (!Controller)return TOptional<UMAiObserver*>();

	TArray<UActorComponent*> Components;
	Controller->GetComponents(UMAiObserver::StaticClass(), Components, true);
	if (Components.Num() == 0)return TOptional<UMAiObserver*>();

	return Cast<UMAiObserver>(Components[0]);
}

void UMAiSensor::RegisterWithObserver(UMAiObserver* Observer)
{
	if (!Observer) return;
	if (SensorGroups.Num() > 0)
	{
		for (auto SensorGroup : SensorGroups)
		{
			Observer->RegisterSensor(this, SensorGroup);
		}
	}
	else
	{
		Observer->RegisterSensor(this, TOptional<FName>());
	}
}

void UMAiSensor::InitializeMAiSensor()
{
	// Register this sensor with all groups
	const auto ObserverState = AMAiInternalObserverState::GetOrCreateObserverState(GetOwner());
	for (const auto Group : SensorGroups)
	{
		ObserverState->RegisterSensor(this, Group);
	}

	// If the parent is an observer, register with the parent
	if (const auto Observer = GetParentObserver())
	{
		RegisterWithObserver(*Observer);
	}
}

void UMAiSensor::BeginPlay()
{
	Super::BeginPlay();
	InitializeMAiSensor();
}

void UMAiSensor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::EndPlayInEditor || EndPlayReason == EEndPlayReason::Quit) return;

	const auto ObserverState = AMAiInternalObserverState::GetOrCreateObserverState(GetOwner());
	ObserverState->RemoveSensor(this);
}

void UMAiSensor::OnDetect(AActor* OtherActor)
{
	OnChange.Broadcast(OtherActor, true);
}

void UMAiSensor::OnLost(AActor* OtherActor)
{
	OnChange.Broadcast(OtherActor, false);
}
