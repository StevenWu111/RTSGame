// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Command_Center.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ACommand_Center : public ABuilding
{
	GENERATED_BODY()

public:
	ACommand_Center();
	UPROPERTY(EditAnywhere)
	int Wood = 100;
	UPROPERTY(EditAnywhere)
	int Metal = 150;
	
};
