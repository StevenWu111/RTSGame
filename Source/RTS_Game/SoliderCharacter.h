// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"

#include "SoliderCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ASoliderCharacter : public ANPC
{
	GENERATED_BODY()
public:
	ASoliderCharacter();
	virtual void BeginPlay() override;
	virtual void Destination() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetSelection() override;
	virtual void SetDeselected() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapOver(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);



	
	
	
	
};


