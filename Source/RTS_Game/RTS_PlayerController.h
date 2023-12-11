// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "DrawDebugHelpers.h"
#include "CoreMinimal.h"
#include "WorkerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "RTS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTS_GAME_API ARTS_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ARTS_PlayerController();

	//Called when game start
	virtual void BeginPlay() override;

	//Use this to setup the input but we currently setup the input in player class
	virtual void SetupInputComponent() override;

	//Use this to spawn the units from server
	UFUNCTION(Server, Reliable)
	void ServerSpawnNPCUnit(TSubclassOf<AActor> Actor, FVector SpawnActorLocation, FRotator Rotation, ABuilding* OwningBuilding);

	//Use this to spawn the units from server, but will called from player
	UFUNCTION(Server, Reliable)
	void ServerSpawnNPCUnitByPlayer(TSubclassOf<AActor> Actor, FVector SpawnActorLocation, FRotator Rotation, APlayerController* OwningPlayerController);

	//Calculate the damage for player when their unit are attacking a building
	UFUNCTION(Server, Reliable)
	void ServerDealDamageToBuilding(ANPC* DamageOwner, ABuilding* DamageReceiver);
	UFUNCTION(Server, Reliable)
	void ServerDealDamageToUnit(ANPC* DamageOwner, ANPC* DamageReceiver);
	

	//Spawn the particles on server and multicast to all the clients (Unreliable function)
	UFUNCTION(Server, Unreliable)
	void ServerSpawnParticles(FVector ParticleSpawnLocation, UNiagaraSystem* ParticleEffect, FRotator SpawnRotation);
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnParticles(FVector ParticleSpawnLocation, UNiagaraSystem* ParticleEffect, FRotator SpawnRotation);

	//server and multicast pause game function, but currently has problem on client side
	UFUNCTION(Server, Reliable)
	void ServerPauseGame(bool bIsPause);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPauseGame(bool bIsPause);

	//UI for winner
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WonUI;

	//UI for loser
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> LoseUI;

	//Make sure server handle the game result for us but execute on the client who trigger it.
	UFUNCTION(Server, Reliable)
	void ServerAddEndGameUI(bool bIsWinner);
	UFUNCTION(Client, Reliable)
	void ClientAddEndGameUI(bool bIsWinner);


	//make sure the ChangeColor function is executed on server and multicast to all client (for the buildings)
	UFUNCTION(Server, Unreliable)
	void ServerChangeColor(ABuilding* Building, bool bIsValidColor);

	//use this to clear the timer
	UFUNCTION(Server, Reliable)
	void ServerClearTimer(FTimerHandle UnitTimer);

	UFUNCTION(Server, Reliable)
	void ServerAddResourcesToWorker(AWorkerCharacter* CurrWorker, FVector ResourcesCenterLocation);

	UFUNCTION(Server, Reliable)
	void ServerClearTheCarryingAmount(AWorkerCharacter* CurrWorker, AMulti_PlayerState* CurrPlayerState);

	void SpawnProjectile(ANPC* Attacker);

	UFUNCTION(Server, Reliable)
	void ServerMakeExplosion(FVector Location, float Damage);
	
};
