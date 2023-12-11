// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Building.h"
#include "MetalResourcesPoint.h"
#include "Metal_Resource.h"
#include "NPC.h"
#include "OilResourcesPoint.h"
#include "Wood_Resource.h"
#include "ClickableActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTS_GAME_API UClickableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClickableActorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//This function will be bind with event on clicked
	UFUNCTION()
	void OnMouseClicked(AActor* TouchedActor, FKey ButtonPressed);
	
	//This function will be bind with event on cursor hover
	UFUNCTION()
	void OnMouseHover(AActor* TouchedActor);
	
	//This function will be bind with event on cursor hover over
	UFUNCTION()
	void OnMouseHoverOver(AActor* TouchActor);

	//Use this function to make the selection shader appear or not
	void ChangeCustomDepth(bool ActiveSelectionShader, int StencilValue);

	//Is is the player clicked this actor or not
	bool IsClicked = false;

	//Use this to store all the static mesh components
	TArray<UActorComponent*> MeshComponents;

	//This is the current owning building (only be valid when GetOwner is a Building)
	ABuilding* CurrentBuilding;

	//This is the current owning Unit (Only valid when Owner is a Unit)
	ANPC* CurrentUnit;
	
	AOilResourcesPoint* CurrWoodResource;

	AMetalResourcesPoint* CurrMetalResource;
	bool bIsBuildingCanClick;
	
		
};
