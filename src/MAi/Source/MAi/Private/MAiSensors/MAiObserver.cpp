// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiSensors/MAiObserver.h"

#include "MAiSensors/Internal/MAiInternalObserverState.h"

void UMAiObserver::BeginPlay()
{
	Super::BeginPlay();

	// Register this observer
	const auto ObserverState = AMAiInternalObserverState::GetOrCreateObserverState(GetOwner());
	ObserverState->RegisterObserver(this);
}

void UMAiObserver::RegisterSensor(UMAiSensor* Sensor, TOptional<FName> InSensorGroup)
{
	auto Binding = Sensor->OnChange.AddLambda([=](AActor* OtherActor, bool Detected)
	{
		SensorEvent(InSensorGroup, OtherActor, Detected);
	});
	Sensors.Add(FMAiObserved{
		Sensor,
		InSensorGroup,
		Binding
	});
}

void UMAiObserver::SensorEvent(TOptional<FName> SensorGroup, AActor* OtherActor, bool Detected)
{
	UE_LOG(LogTemp, Display, TEXT("Observer change: %s: %s"), *OtherActor->GetName(), Detected ? "Yes" : "No");
}

void UMAiObserver::RemoveSensor(UMAiSensor* Sensor, FName SensorGroup)
{
	TArray<FMAiObserved> StillObserved;
	for (const auto Observed : Sensors)
	{
		if (Observed.Sensor == Sensor && Observed.SensorGroup == SensorGroup)
		{
			Sensor->OnChange.Remove(Observed.Binding);
			continue;
		}
		StillObserved.Add(Observed);
	}
	Sensors = StillObserved;
}

void UMAiObserver::ClearSensors()
{
}

void UMAiObserver::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (EndPlayReason == EEndPlayReason::EndPlayInEditor || EndPlayReason == EEndPlayReason::Quit) return;

	const auto ObserverState = AMAiInternalObserverState::GetOrCreateObserverState(GetOwner());
	ObserverState->RemoveObserver(this);
}
