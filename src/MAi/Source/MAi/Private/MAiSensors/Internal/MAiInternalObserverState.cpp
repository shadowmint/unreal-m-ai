// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiSensors/Internal/MAiInternalObserverState.h"

#include "Kismet/GameplayStatics.h"

TWeakObjectPtr<AMAiInternalObserverState> AMAiInternalObserverState::Cached;

TWeakObjectPtr<AMAiInternalObserverState> AMAiInternalObserverState::GetOrCreateObserverState(UObject* WorldContextObject)
{
	if (Cached.IsValid())
	{
		return Cached;
	}
	auto Instance = Cast<AMAiInternalObserverState>(UGameplayStatics::GetActorOfClass(WorldContextObject, StaticClass()));
	if (Instance == nullptr)
	{
		auto Params = FActorSpawnParameters();
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.bNoFail = true;
		Instance = WorldContextObject->GetWorld()->SpawnActor<AMAiInternalObserverState>(Params);
		Cached = Instance;
	}
	return Instance;
}

void AMAiInternalObserverState::RegisterSensor(UMAiSensor* Sensor, const FName& SensorGroup)
{
	auto Group = GetSensorGroup(SensorGroup);
	Group.Add(Sensor);
	for (const auto Observer : Observers)
	{
		if (Observer->SensorGroups.Contains(SensorGroup))
		{
			Observer->RegisterSensor(Sensor, SensorGroup);
		}
	}
}

void AMAiInternalObserverState::RegisterObserver(UMAiObserver* Observer)
{
	if (Observer) return;
	Observers.Add(Observer);
	for (const auto Group : Observer->SensorGroups)
	{
		if (Sensors.Contains(Group))
		{
			this->RegisterAllSensors(Observer, Group, Sensors[Group]);
		}
	}
}

void AMAiInternalObserverState::RemoveSensor(UMAiSensor* Sensor)
{
	for (auto const GroupName : Sensor->SensorGroups)
	{
		auto Group = GetSensorGroup(GroupName);
		Group.Remove(Sensor);

		for (const auto Observer : Observers)
		{
			if (Observer->SensorGroups.Contains(GroupName))
			{
				Observer->RemoveSensor(Sensor, GroupName);
			}
		}
	}
}

void AMAiInternalObserverState::RemoveObserver(UMAiObserver* Observer)
{
	this->Observers.Remove(Observer);
}

TArray<TWeakObjectPtr<UMAiSensor>>& AMAiInternalObserverState::GetSensorGroup(FName Name)
{
	if (!Sensors.Contains(Name))
	{
		Sensors[Name] = TArray<TWeakObjectPtr<UMAiSensor>>();
	}
	return Sensors[Name];
}

void AMAiInternalObserverState::RegisterAllSensors(UMAiObserver* Observer, FName GroupName, TArray<TWeakObjectPtr<UMAiSensor>>& SensorGroup)
{
	for (const auto Sensor : SensorGroup)
	{
		if (Sensor.IsValid())
		{
			Observer->RegisterSensor(Sensor.Get(), GroupName);
		}
	}
}
