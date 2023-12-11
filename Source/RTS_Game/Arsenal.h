// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Building_Widget.h"
#include "NPC.h"
#include "Arsenal.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API AArsenal : public ABuilding
{
	GENERATED_BODY()

public:
	AArsenal();
	virtual void BeginPlay() override;

	//Build
	UPROPERTY(EditAnywhere)
	int Wood = 100;
	UPROPERTY(EditAnywhere)
	int Metal = 150;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//UI
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Widget;
	
	UBuilding_Widget*  WidgetInstance;
	virtual void SetSelection() override;
	virtual void SetDeselection() override;


	// Generate more Worker
	UPROPERTY(EditAnywhere, Category = "Type")
	TSubclassOf<ANPC> Solider;
	virtual void Function() override;

	FVector GatherLocation=FVector::ZeroVector;
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetLocation(const FVector Location) override;

	UPROPERTY(EditAnywhere)
	float SoldierOilCost = 80;
	UPROPERTY(EditAnywhere)
	float SoldierMetalCost = 50;
	
};
