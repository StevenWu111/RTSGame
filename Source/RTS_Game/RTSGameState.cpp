// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSGameState.h"

#include "AIController.h"
#include "RTSCharacter.h"
#include "RTS_PlayerController.h"

ARTSGameState::ARTSGameState()
{
	
}

void ARTSGameState::BeginPlay()
{
	Super::BeginPlay();
	ValidColour = FLinearColor::Green;
	InValidColour = FLinearColor::Red;
}

void ARTSGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	UE_LOG(LogTemp,Warning,TEXT("Cast to Player"))
	AMulti_PlayerState* NewRTSPlayerState = Cast<AMulti_PlayerState>(PlayerState);
	if (NewRTSPlayerState)
	{
		if (TeamOne.Num() < TeamTwo.Num())
		{
			TeamOne.Add(NewRTSPlayerState);
			UE_LOG(LogTemp,Warning,TEXT("Added To Team 1"))
		}
		else if(TeamTwo.Num() < TeamOne.Num())
		{
			TeamTwo.Add(NewRTSPlayerState);
			UE_LOG(LogTemp,Warning,TEXT("Added To Team 2"))
		}
		else
		{
			if (rand()%2==0)
			{
				TeamOne.Add(NewRTSPlayerState);
				UE_LOG(LogTemp,Warning,TEXT("Added To Team 1"))
			}
			else
			{
				TeamTwo.Add(NewRTSPlayerState);
				UE_LOG(LogTemp,Warning,TEXT("Added To Team 2"))
			}
		}
		UE_LOG(LogTemp,Warning,TEXT("Added To Team"))
	}
}

