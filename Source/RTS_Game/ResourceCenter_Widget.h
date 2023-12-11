// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building_Widget.h"
#include "ResourceCenter_Widget.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API UResourceCenter_Widget : public UBuilding_Widget
{
	GENERATED_BODY()

public:

	virtual void ButtonClicked() override;
	virtual void NativeConstruct() override;
	
};
