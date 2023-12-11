// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Metal_Resource.h"
#include "Wood_Resource.h"
#include "GameFramework/Actor.h"
#include "SpawnResource.generated.h"

UCLASS()
class RTS_GAME_API ASpawnResource : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnResource();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UPROPERTY(EditAnywhere, Category = "Entities")
	float MaxTree = 10;
	
	UPROPERTY(EditAnywhere, Category = "Entities")
	float MaxMetal = 10;
	

	UPROPERTY(EditAnywhere, Category = "Entities")
	TSubclassOf<AWood_Resource> TreeClass;

	UPROPERTY(EditAnywhere, Category = "Entities")
	TSubclassOf<AMetal_Resource> MetalClass;
	
	int NumTree = 0;
	int NumMetal = 0;
	
	//TArray<AWood_Resource*> Tree_Count;

	void SpawnTree();
	void SpawnMetal();

};
