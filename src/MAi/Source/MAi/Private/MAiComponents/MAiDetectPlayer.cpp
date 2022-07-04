// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiComponents/MAiDetectPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/MComponent.h"


UMAiDetectPlayer::UMAiDetectPlayer()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMAiDetectPlayer::BeginPlay()
{
	Super::BeginPlay();

	Perception = FMComponent::GetComponent<UAIPerceptionComponent>(GetOwner());

	CanCurrentlySeePlayer = false;
	if (Perception.IsValid())
	{
		Perception->OnPerceptionUpdated.AddDynamic(this, &UMAiDetectPlayer::OnPerceptionUpdated);
		IsReady = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UMAiDetectPlayer::BeginPlay: Not running on a valid AIController"));
		IsReady = false;
	}
}

void UMAiDetectPlayer::ApplyLostSightOfPlayer(UBlackboardComponent* Blackboard) const
{
	switch (OnDetectionLost)
	{
	case EMAiDetectPlayerOnFalse::SetBlackboardKeyToFalse:
		Blackboard->SetValueAsBool(SetPlayerVisibleBlackboardKeyBoolean, false);
		Blackboard->SetValueAsObject(SetPlayerRefBlackboardKeyObject, nullptr);
		break;
	case EMAiDetectPlayerOnFalse::ClearBlackboardKey:
		Blackboard->ClearValue(SetPlayerVisibleBlackboardKeyBoolean);
		Blackboard->ClearValue(SetPlayerRefBlackboardKeyObject);
		break;
	default: ;
	}
}

void UMAiDetectPlayer::ApplyGainedSightOfPlayer(APawn* const CanCurrentlySee, UBlackboardComponent* Blackboard) const
{
	Blackboard->SetValueAsBool(SetPlayerVisibleBlackboardKeyBoolean, true);
	Blackboard->SetValueAsObject(SetPlayerRefBlackboardKeyObject, CanCurrentlySee);
}

void UMAiDetectPlayer::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (!IsReady) return;

	auto const CanCurrentlySee = CanCurrentlyPerceivePlayer();
	if (!!CanCurrentlySee != CanCurrentlySeePlayer)
	{
		CanCurrentlySeePlayer = !!CanCurrentlySee;

		UBlackboardComponent* Blackboard = nullptr;
		auto const Parent = Cast<AAIController>(GetOwner());
		if (Parent)
		{
			Blackboard = Parent->GetBlackboardComponent();
		}

		if (Blackboard)
		{
			if (CanCurrentlySee)
			{
				ApplyGainedSightOfPlayer(CanCurrentlySee, Blackboard);
			}
			else
			{
				LostSightCoolDownActive = true;
				SecondsSinceLostPlayer = 0;
			}
		}
	}
}

APawn* UMAiDetectPlayer::CanCurrentlyPerceivePlayer() const
{
	if (!Perception.IsValid()) return nullptr;

	auto const Controller = UGameplayStatics::GetPlayerController(this, PlayerId);
	if (!Controller) return nullptr;

	auto const Pawn = Controller->GetPawn();
	if (!Pawn) return nullptr;

	auto Info = FActorPerceptionBlueprintInfo();
	Perception->GetActorsPerception(Pawn, Info);
	if (Info.LastSensedStimuli.Num() == 0) return nullptr;

	for (const auto& S : Info.LastSensedStimuli)
	{
		if (S.WasSuccessfullySensed() && !S.IsExpired())
		{
			return Pawn;
		}
	}

	return nullptr;
}

// Called every frame
void UMAiDetectPlayer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!LostSightCoolDownActive) return;

	SecondsSinceLostPlayer += DeltaTime;
	if (SecondsSinceLostPlayer >= LostSightCoolDown)
	{
		LostSightCoolDownActive = false;
		SecondsSinceLostPlayer = 0;

		auto const Parent = Cast<AAIController>(GetOwner());
		if (Parent)
		{
			const auto Blackboard = Parent->GetBlackboardComponent();
			if (Blackboard)
			{
				ApplyLostSightOfPlayer(Blackboard);
				if (Debug)
				{
					UE_LOG(LogTemp, Display, TEXT("UMAiDetectPlayer::TickComponent: Lost sight of player"));	
				}
			}
		}
	}
}
