// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "RTSGameState.h"
#include "Building.generated.h"

UCLASS()
class RTS_GAME_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();

	UPROPERTY(EditAnywhere, Replicated)
	float BuildingHP;

	//This is the reference of our customized gamestate
	ARTSGameState* GameState;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* BuildingMesh;

	//The collision box of the building
	UPROPERTY(EditAnywhere)
	UBoxComponent*BuildCollision;
	
	//This is the material that we will use to change the color when the building in the
	//placement mode
	UPROPERTY(EditAnywhere)
	UMaterial* MaterialClass;

	void ChangeColor(bool bIsValid);

	//Use this to tell all the client we need set the collisions of the placeable building to overlap
	UFUNCTION(Server, Reliable)
	void ServerSetPlaceableBuildingCollision();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPlaceableBuildingCollision();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastChangeColor(bool bIsValid);

	//This is the material instance that we created 
	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;
	
	//This will be called when overlap happened, and update the valid or invalid material
	void UpdateState();
	
	//Use this to store all the static mesh components of current building
	TArray<UStaticMeshComponent*> MeshComponents;
	
	//This function will loop through all the static mesh component and set the material to it
	void ChangeMaterial();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//This will store the code of the functionality of this building
	virtual void Function();

	//This is used to add the UI to the viewport or destroy the UI from the vireport
	virtual void SetSelection();
	virtual void SetDeselection();

	//This is the cost of place this building
	int WoodCost;
	int MetalCost;

	//Use this to set the building location when player in the placement mode
	virtual void SetLocation(const FVector Location);

	//This is the player state that belongs to the owner player of this building
	UPROPERTY(Replicated)
	AController* OwningController;

	//Use this to replicated the variables
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	//This will return the player Controller of the current local player
	AController* GetCurrentLocalController();

	//use this function to assign the owning controller to buildings
	void SetOwningController(AController* Controller);

	//check is building is currently in placement mode or not, by default it is true because we assume all the buildings
	//need to place first. This can avoid the units attack the building when player are placing them
	bool bIsPlacing = true;

};
