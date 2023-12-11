// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Components/ActorComponent.h"
#include "RTSGameState.h"
#include "Net/UnrealNetwork.h"
#include "placableActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTS_GAME_API UplacableActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UplacableActorComponent();
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", Replicated)
	//UMaterialInstance* PloppableMaterial;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material", Replicated)
	//UMaterialInstance* InValidMaterial;
	ABuilding* OwnerBuilding;
	
	bool CanPlop = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	bool IsPlacementValid = true;
	UFUNCTION()
	void OnOverlapBegin(AActor* Owner, AActor* OtherActor);
	UFUNCTION()
	void OnOverlapEnd(AActor* Owner, AActor* OtherActor);


	

	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
