// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "GameFramework/HUD.h"
#include "RTS_Hud.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ARTS_Hud : public AHUD
{
	GENERATED_BODY()
public:
	//Position of mouse on screen when pressed
	FVector2d InitialPoint; 
	
	//Position of mouse on screen while holding
	FVector2d CurrentPoint;


	FVector2d GetMousePosition();


	//Same as Tick();
	void DrawHUD() override;

	bool StartSelection=false;


	TArray<ANPC*> GetActor;
	
	

	void Select();
	void Cancel();

	//ABuilding* GetBuilding;
};