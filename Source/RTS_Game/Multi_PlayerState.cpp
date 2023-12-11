// Fill out your copyright notice in the Description page of Project Settings.


#include "Multi_PlayerState.h"

#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

void AMulti_PlayerState::ClientInitialize(AController* C)
{
	Super::ClientInitialize(C);

	if(HasAuthority())
    {
		GetWorld()->GetGameState()->AddPlayerState(this);
    }

}

void AMulti_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMulti_PlayerState, WoodCount);
	DOREPLIFETIME(AMulti_PlayerState, MetalCount);
}
