// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Multi_PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API AMulti_PlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void ClientInitialize(AController* C) override;

	UPROPERTY(Replicated)
	int MetalCount = 300;
	UPROPERTY(Replicated)
	int WoodCount = 300;

	virtual  void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
