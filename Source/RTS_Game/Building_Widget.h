// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"  // Include for UProgressBar
#include "Components/TextBlock.h"    // Include for UTextBlock
#include "Building_Widget.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API UBuilding_Widget : public UUserWidget
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentHealthLabel;

public:

	UFUNCTION(BlueprintCallable)
    virtual void ButtonClicked();

	bool StartGenerateSolider=false;
	bool StartGenerateWorker=false;

	ABuilding* Building;
	void NativeTick (const FGeometry& MyGeometry, float InDeltaTime) override;
};
