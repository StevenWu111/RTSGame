// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSCharacter.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "MainUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API UMainUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct () override;
	void NativeTick (const FGeometry& MyGeometry, float InDeltaTime) override;
	ARTSCharacter* Player;


protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MetalCount;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WoodCount;
	
};
