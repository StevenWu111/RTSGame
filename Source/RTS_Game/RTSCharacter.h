// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "RTS_PlayerController.h"
#include "RTS_Hud.h"
#include "EnhancedInputComponent.h"
#include "Multi_PlayerState.h"
#include "NPC.h"
#include "PlayerPlaceRange.h"
#include "ResourcesPoint.h"
#include "WorkerCharacter.h"
#include "RTSCharacter.generated.h"

class UCapsuleComponent;
class UInputMappingContext;



UCLASS()
class RTS_GAME_API ARTSCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveADAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HoldRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RotatePitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightClick;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float MoveSpeed = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float AccSpeed = 4000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	float ZoomSpeed= 50.f;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraArm;
	
    UPROPERTY(EditAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
	float ArmMax = 4000;

	UPROPERTY(EditAnywhere)
	float ArmMin = 500;


public:
	// Sets default values for this character's properties
	ARTSCharacter();

	//A reference of our current player controller
	ARTS_PlayerController* PlayerController;
	
	//save the building that we currently selected
	ABuilding* SelectedBuilding;
	ABuilding* PreviousBuilding;

	const ARTSGameState* GameState;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//Function that bind with Input Action
	void Move(const FInputActionValue& Value);
	void MoveAD(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void MoveUp(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
	void HoldingRight(const FInputActionValue& Value);
	void ReleaseRight(const FInputActionValue& Value);
	void CameraZoom(const FInputActionValue& Value);
	void RotatePitch(const FInputActionValue& Value);
	void LeftPressed(const FInputActionValue& Value);
	void LeftRelease(const FInputActionValue& Value);
	void RightPressed(const FInputActionValue& Value);
	void BindSpawnBuildingWithLeftClick(const FInputActionValue& Value);

	//Player is holding right mouse button or not
	bool bIsMidButtonHold = false;

	//Store the player viewport size
	FVector2d ViewportSize = FVector2d(0,0);

	//A reference of our customized HUD
	ARTS_Hud* Hud;
	
	//A reference of our Units that we currently selected
	TArray<ANPC*> SelectedActor;

	//A reference of out units that we selected and can controlled
	TArray<ANPC*> ControlledUnit;

	//Use this to spawn the initial units for the players
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ANPC>> InitialUnits;

	//Use this to spawn the command center when the game start
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABuilding> CommandCenter;

	//Use this to spawn the place range for players
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> PlaceRangeClass;

	APlayerPlaceRange* PlaceRange;
	
	//Use this to set the selection shader
	void ChangeCustomDepth(bool ActiveSelectionShader, AActor* CurrentActor, int StencilValue);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Place actor during game play
	UPROPERTY(Replicated)
	bool PlacementStatus = false;

	//The placeable actor that we currently hold. (The building currently in placement mode)
	UPROPERTY(Replicated)
	AActor* PlaceablActor;

	//The spawn building type
	UPROPERTY(Replicated)
	TSubclassOf<ABuilding> PlaceableActorClass;

	//The building location, use this to make sure the building is following player cursor
	UPROPERTY(Replicated)
	FVector BuildingLocation;

	//Let the server set the building location variable
	UFUNCTION(Server, Reliable)
	void ServerSetBuildingLocationVar(const FVector Location);
	//Let the server change the building location to the building location variable
	UFUNCTION(Server, Reliable)
	void ServerSetBuildingLocation(ABuilding* Building);

	UFUNCTION(Client, Reliable)
	void ClientSetBuildingLocation(ABuilding* Building);
	//Actual function of set building location, will be called in server side to make sure
	//client side can inform the server and update the building location
	void PlaceLocation();

	//Spawn the building on server side
	UFUNCTION(Server, Reliable)
	void ServerSpawnBuilding(TSubclassOf<ABuilding> NewBuilding, FVector NewBuildingLocation, FRotator NewBuildingRotation);
	
	//Actual function of spawn building
	void SpawnBuilding();

	//Infrom the server we set placement mode enable
	UFUNCTION(Server, Reliable)
	void ServerSetPlaceEnable(bool IsEnabled, TSubclassOf<ABuilding> Building);
	
	//Infrom server that we want to spawn a placeable building
	UFUNCTION(Server, Reliable)
	void ServerSpawnPlaceableBuilding(FVector Location);
	
	//Actual function to enable the placement mode 
	void SetPlaceEnable(bool IsEnabled, TSubclassOf<ABuilding> Building);

	UFUNCTION(Server, Reliable)
	void ServerCalculateCost(float MetalNum, float WoodNum);

	//a reference of player game state
	AMulti_PlayerState* PlayerState;
	
	//Use this variable to set the workers target location (to construct building)
	FVector SpawnBuildLocation;

	//use for make AI controller work for RPC, this variable will be update when player
	//right click on the ground (the target location used for navigation)
	UPROPERTY(Replicated)
	FVector TargetLocation;

	//Let server set the Worker target location variable
	UFUNCTION(Server, Reliable)
	void ServerSetLocationVar(const FVector Location);
	
	//Let the server move the Worker AI to the target location for us
	UFUNCTION(Server, Reliable)
	void ServerSetNewLocation(ANPC* Unit);

	//Use this to replicate the variables that we created before
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//Server handle the placement status (in placement mode or not)
	UFUNCTION(Server, Reliable)
	void ServerSetPlacementStatus(bool bSetPlaceMode);

	//Let server know we want to destroy the actor
	UFUNCTION(Server, Reliable)
	void ServerDestroyActor(AActor* Actor);

	// Make server assign the owner to the building
	UFUNCTION(Server, Reliable)
	void ServerSetOwningPlayer(ABuilding* Building);

	//this is the niagara particle system that will be spawn when player pressed right button and move the units
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FXCursor;

	//multicast the particle effect to all the connected client
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnCursorParticles(UNiagaraSystem* NiagaraSystem);

	//Use these two functions to call the attack function inside of ANPC class from server (Based on target is a building or a unit)
	UFUNCTION(Server, Reliable)
	void ServerAttackTarget(ANPC* AttackUnit, AActor* Target);

	UFUNCTION(Server, Reliable)
	void ServerAddPlaceableComp();

	UFUNCTION(Server, Reliable)
	void ServerSpawnActor(TSubclassOf<AActor> SpawnActor, FVector SpawnLocation, FRotator SpawnRotation);

	UFUNCTION(Server, Reliable)
	void ServerGatheringResources(AWorkerCharacter* CurrWorker, AResourcesPoint* ResourcesPoint);

	UPROPERTY(Replicated)
	TArray<ABuilding*> PlayerSpawnedBuildings;

	UFUNCTION(NetMulticast, Unreliable)
	void MultiCastClearFoliage(AActor* Actor);

	UFUNCTION(Server, Reliable)
	void ServerCreateUI();

	UFUNCTION(Client, Reliable)
	void ClientCreateUI();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> MainUI;

	UFUNCTION(Server, Reliable)
	void ServerClearTarget(ANPC* Unit);
	
};

