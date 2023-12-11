// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Multi_PlayerState.h"
#include "RTSGameState.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ARTSGameState : public AGameStateBase
{
	GENERATED_BODY()
	ARTSGameState();
public:
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere)
	FLinearColor ValidColour;
	UPROPERTY(EditAnywhere)
	FLinearColor InValidColour;

	UPROPERTY(EditAnywhere)
	TArray<AMulti_PlayerState*> TeamOne;
	UPROPERTY(EditAnywhere)
	TArray<AMulti_PlayerState*> TeamTwo;

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	
};
