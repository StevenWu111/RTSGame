// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "RTSAIController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ARTSAIController : public AAIController
{
	GENERATED_BODY()

	ARTSAIController();
public:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void MoveToTargetLocation(FVector Location);
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};


