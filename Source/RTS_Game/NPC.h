// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Building.h"
#include "NPC.generated.h"

//This is the Enum that shows the current Unit status
UENUM()
enum EStatus
{
	Default,
	AttackingTargetEnemy,
	CollectingResources
};

enum EAgentType
{
	Melee,
	Ranger
};

UCLASS()
class RTS_GAME_API ANPC : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Use these two function to add selection UI or destroy selection UI
	virtual void SetSelection();
	virtual void SetDeselected();

	//These two will not be use for now
	virtual void Destination();
	FVector Goal;

	//This is the UI class for the current building 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<UUserWidget> Widget;

	//This is the UI instance that we will create from the previous UI class
	UUserWidget*  WidgetInstance;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	USphereComponent* AttackRange;

	//This is the player controller that spawned us
	UPROPERTY(Replicated)
	AController* ControlledController;
	
	UPROPERTY(EditAnywhere)
	float AttackDamage;

	UPROPERTY(EditAnywhere)
	float AttackFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Replicated)
	float UnitHP = 100.0f;

	UPROPERTY(EditAnywhere)
	float RotationSpeed = 20.0f;

	//Use this to replicate the variables that we created before
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Use this to move the Units
	void MoveToTargetLocation(const FVector Location);

	//Use this to set the attack order to this unit
	void AttackTarget(AActor* TargetEnemy);

	//Use this to store the enemy actor that we currently targeting
	UPROPERTY(Replicated)
	AActor* TargetEnemyRef;

	void MakeServerCalculateDamage();

	//This will save the Unit current status, every unit will start at default
	EStatus CurrentStatus = Default;

	//The timer used to calculate the attack frequency and the function to clear the timerhandle after finished attack
	FTimerHandle TimerHandle;
	void ClearTimer();

	FTimerHandle GatheringTimerHandle;

	EAgentType AgentType;
	
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* AttackParticles;

	//This will return the player Controller of the current local player
	AController* GetCurrentLocalController();

	//Use this to record the attackable enemies
	TArray<ABuilding*> AttackBuildingList;
	TArray<ANPC*> AttackUnitList;

	//The unit will attack any enemy unit inside of attack range
	UPROPERTY(EditAnywhere)
	bool bAttackFreely = true;

	//The unit will always chasing the target enemy when they are out of range
	UPROPERTY(EditAnywhere)
	bool bChase = false;

	bool bIsSelect = false;

	void RotateTowardEnemy(float DeltaTime);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABullet> ProjectileClass;


	
};


