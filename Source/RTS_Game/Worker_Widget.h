// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RTSCharacter.h"
#include "Worker_Widget.generated.h"




/**
 * 
 */
UCLASS()
class RTS_GAME_API UWorker_Widget : public UUserWidget
{
	GENERATED_BODY()


public:


	
	void NativeConstruct () override;
	void NativeTick (const FGeometry& MyGeometry, float InDeltaTime) override;
	


	UFUNCTION()
	void PlaceResourceCenter();
	UFUNCTION()
	void PlaceArsenal();
	
	ARTSCharacter* Player;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> ResourceCenter_Build;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> Arsenal_Build;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> Command_Center_Build;

	
	

};
