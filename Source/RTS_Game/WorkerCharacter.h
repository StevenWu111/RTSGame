// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC.h"
#include "ResourcesPoint.h"
#include "Multi_PlayerState.h"
#include "WorkerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API AWorkerCharacter : public ANPC
{
	GENERATED_BODY()
public:
	AWorkerCharacter();
	//This is the attack range for our Units
	
	
	virtual void BeginPlay() override;
	virtual void Destination() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetSelection() override;
	virtual void SetDeselected() override;

	
	UPROPERTY(EditAnywhere, Replicated)
	float MaxGather=10.0f;
	

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapOver(UPrimitiveComponent* OverlappedComp, 
   AActor* OtherActor, 
   UPrimitiveComponent* OtherComp, 
   int32 OtherBodyIndex);

	//Use this array to track the resources point that we currently overlapped
	TArray<AResourcesPoint*> InRangeResourcesPoints;
	//save the target Resources point
	UPROPERTY(Replicated)
	AResourcesPoint* TargetResourcesPoint;
	//use this function to collect the resources
	void CollectResources(AResourcesPoint* ResourcesPoint, TArray<ABuilding*> PlayerSpawnedBuildings);
	//this is to track the 
	bool bStartGatherResources = false;
	//use this to track the current resources amount on this worker and the resources type
	UPROPERTY(Replicated)
	float CurrCarryingAmount;
	EResourcesType CurrResourcesType = None;
	//Use this to set the gathering speed
	UPROPERTY(EditAnywhere, Replicated)
	float GatheringSpeed = 1.0f;
	//use this function to make server add the resources to our worker
	void MakerServerAddResourcesToWorker();
	//Use this to replicate the variables that we created before
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ABuilding* WorkerBackTargetBuilding;





	
};


