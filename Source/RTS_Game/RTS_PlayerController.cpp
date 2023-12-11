// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_PlayerController.h"

#include "Bullet.h"
#include "Command_Center.h"
#include "NiagaraFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ARTS_PlayerController::ARTS_PlayerController()
{

	bShowMouseCursor=true;
	bEnableMouseOverEvents = true;
}

void ARTS_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARTS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}


void ARTS_PlayerController::ServerClearTheCarryingAmount_Implementation(AWorkerCharacter* CurrWorker, AMulti_PlayerState* CurrPlayerState)
{
	if (HasAuthority())
	{
		if (CurrPlayerState)
		{
			switch (CurrWorker->CurrResourcesType)
			{
			case Metal:
				CurrPlayerState->MetalCount += CurrWorker->CurrCarryingAmount;
				break;
			case Oil:
				CurrPlayerState->WoodCount += CurrWorker->CurrCarryingAmount;
				break;
			default:
				break;
			}
		}
		CurrWorker->CurrCarryingAmount = 0;
		CurrWorker->CurrResourcesType = None;
	}
}

void ARTS_PlayerController::ServerAddResourcesToWorker_Implementation(AWorkerCharacter* CurrWorker, FVector ResourcesCenterLocation)
{
	//if we cannot carry more resources
	if (CurrWorker->CurrCarryingAmount >= CurrWorker->MaxGather)
	{
		//we clear the timer
		ServerClearTimer(CurrWorker->GatheringTimerHandle);
		GEngine->AddOnScreenDebugMessage(1,3.f,FColor::Blue,TEXT("You cannot gather more resources"));
		if (CurrWorker->WorkerBackTargetBuilding)
		{
			CurrWorker->MoveToTargetLocation(ResourcesCenterLocation);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(2,3.f,FColor::Blue,TEXT("There is no resoucres center"));
		}
	}
	else if (CurrWorker->CurrResourcesType == None && CurrWorker->TargetResourcesPoint)
	{
		//if this is our first collect, we set the carrying resources type limit to worker
		CurrWorker->CurrResourcesType = CurrWorker->TargetResourcesPoint->ResourcesType;
		//and we add one to the carry amount
		CurrWorker->CurrCarryingAmount += 1.0f;
		CurrWorker->TargetResourcesPoint->CurrResourcesAmount -= 1.0f;
	}
	else
	{
		//if we can carry more, we add 1 to the carrying mount
		CurrWorker->CurrCarryingAmount += 1.0f;
		CurrWorker->TargetResourcesPoint->CurrResourcesAmount -= 1.0f;
	}
	if (CurrWorker->TargetResourcesPoint && CurrWorker->TargetResourcesPoint->CurrResourcesAmount <= 0)
	{
		CurrWorker->TargetResourcesPoint->Destroy();
		CurrWorker->TargetResourcesPoint = nullptr;
		ServerClearTimer(CurrWorker->GatheringTimerHandle);
		if (CurrWorker->WorkerBackTargetBuilding)
		{
			CurrWorker->MoveToTargetLocation(CurrWorker->WorkerBackTargetBuilding->GetActorLocation());
		}
	}
}


void ARTS_PlayerController::ServerChangeColor_Implementation(ABuilding* Building, bool bIsValidColor)
{
	Building->ChangeColor(bIsValidColor);
}

void ARTS_PlayerController::ServerDealDamageToUnit_Implementation(ANPC* DamageOwner, ANPC* DamageReceiver)
{
	if (HasAuthority())
	{
		//We need handle different type of agents
		switch (DamageOwner->AgentType)
		{
			//if this is a melee unit
			case Melee:
				if (DamageOwner->IsValidLowLevel() && DamageReceiver->IsValidLowLevel())
				{
					//When it has the particles assigned
					if (DamageOwner->AttackParticles)
					{
						//This is the location that we spawn our attack particles effect
						FVector AttackLocation = DamageOwner->GetActorLocation()+DamageOwner->GetActorForwardVector()*5;
						//multicast the particles
						MulticastSpawnParticles(AttackLocation, DamageOwner->AttackParticles, DamageOwner->GetActorRotation());
					}
					// When the damage receiver has HP, the attacker have the damage and attack frequency
					if (DamageReceiver->UnitHP && DamageOwner->AttackDamage)
					{
						//We deal the damage to building
						DamageReceiver->UnitHP -= DamageOwner->AttackDamage;
						//When the building has no health left
						if (DamageReceiver->UnitHP <= 0.0f)
						{
							//we destroy that building
							DamageReceiver->Destroy();
							//and clear the attack timer for the attack unit
							ServerClearTimer(DamageOwner->TimerHandle);
							//We clear the target reference inside of damage owner
							DamageOwner->TargetEnemyRef = nullptr;
						}
					}
				}
			break;
		case Ranger:
			if (DamageOwner)
			{
				//FVector NSSpawnLocation = DamageOwner->GetActorLocation() + DamageOwner->GetActorForwardVector() * 310 + FVector(0,0,120);
				//ServerSpawnReliableParticles(NSSpawnLocation, DamageOwner->AttackParticles, DamageOwner->GetActorRotation());
				SpawnProjectile(DamageOwner);
			}
			break;
		}
	}
}

void ARTS_PlayerController::SpawnProjectile(ANPC* Attacker)
{
	if (Attacker->ProjectileClass)
	{
		const FVector MuzzleOffset = FVector(310.0f, 0.0f, 120.0f);
		FRotator SpawnRotation = Attacker->GetActorRotation();
		const FVector BulletSpawnLocation = Attacker->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
		SpawnRotation.Pitch += FMath::RandRange(-0.5,0.5);
		SpawnRotation.Roll += FMath::RandRange(-0.5,0.5);
		SpawnRotation.Yaw += FMath::RandRange(-0.5,0.5);
		AActor* Temp = GetWorld()->SpawnActor<ABullet>(Attacker->ProjectileClass, BulletSpawnLocation, SpawnRotation);
		if (Temp)
		{
			Cast<ABullet>(Temp)->AttackOwner = Attacker;
			Cast<ABullet>(Temp)->SpawnedControlelr = Cast<ARTS_PlayerController>(Attacker->ControlledController);
		}
	}
}


void ARTS_PlayerController::ServerMakeExplosion_Implementation(FVector Location, float Damage)
{
	if (HasAuthority())
	{
		TArray<FHitResult> HitResults;
		//this is the collision sphere that we will use to do the sweep
		FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(300.0f);
		//draw the debug sphere
		DrawDebugSphere(GetWorld(), Location, CollisionSphere.GetSphereRadius(), 50, FColor::Red, false, 1.0f, 0,0);
		if(GetWorld()->SweepMultiByChannel(HitResults, Location,Location, FQuat::Identity, ECC_Pawn, CollisionSphere)){
			//loop through the results
			for(auto& Hit : HitResults)
			{
				if (ANPC* UnderAttackUnits = Cast<ANPC>(Hit.GetActor()))
				{
					UnderAttackUnits->UnitHP -= Damage;
					if (UnderAttackUnits->UnitHP <= 0.0f)
					{
						UnderAttackUnits->Destroy();
					}
				}
			}
		}
	}
}




void ARTS_PlayerController::ServerClearTimer_Implementation(FTimerHandle UnitTimer)
{
	GetWorld()->GetTimerManager().ClearTimer(UnitTimer);
}

void ARTS_PlayerController::ServerAddEndGameUI_Implementation(bool bIsWinner)
{
	ClientAddEndGameUI(bIsWinner);
}

void ARTS_PlayerController::ClientAddEndGameUI_Implementation(bool bIsWinner)
{
	if (bIsWinner && WonUI)
	{
		UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), WonUI);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
		}
	}
	else
	{
		if (LoseUI)
		{
			UUserWidget* WidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LoseUI);
			if (WidgetInstance)
			{
				WidgetInstance->AddToViewport();
			}
		}
	}
}

void ARTS_PlayerController::ServerPauseGame_Implementation(bool bIsPause)
{
	if (HasAuthority())
	{
		MulticastPauseGame(bIsPause);	
	}
}

void ARTS_PlayerController::MulticastPauseGame_Implementation(bool bIsPause)
{
	UGameplayStatics::SetGamePaused(GetWorld(), bIsPause);
}

void ARTS_PlayerController::ServerSpawnParticles_Implementation(FVector ParticleSpawnLocation, UNiagaraSystem* ParticleEffect, FRotator SpawnRotation)
{
	MulticastSpawnParticles(ParticleSpawnLocation, ParticleEffect, SpawnRotation);
}
void ARTS_PlayerController::MulticastSpawnParticles_Implementation(FVector ParticleSpawnLocation, UNiagaraSystem* ParticleEffect, FRotator SpawnRotation)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleEffect, ParticleSpawnLocation, SpawnRotation);
}

void ARTS_PlayerController::ServerDealDamageToBuilding_Implementation(ANPC* DamageOwner, ABuilding* DamageReceiver)
{
	//Handle only by server
	if (HasAuthority())
	{
		//We need handle different type of agents
		switch (DamageOwner->AgentType)
		{
		//if this is a melee unit
		case Melee:
			//When it has the particles assigned
			if (DamageOwner->AttackParticles)
			{
				//This is the location that we spawn our attack particles effect
				FVector AttackLocation = DamageOwner->GetActorLocation()+DamageOwner->GetActorForwardVector()*5;
				//multicast the particles
				MulticastSpawnParticles(AttackLocation, DamageOwner->AttackParticles, DamageOwner->GetActorRotation());
			}
			// When the damage receiver has HP, the attacker have the damage and attack frequency
			if (DamageReceiver->BuildingHP && DamageOwner->AttackDamage && DamageOwner->AttackFrequency)
			{
				//We deal the damage to building
				DamageReceiver->BuildingHP -= DamageOwner->AttackDamage;
				//When the building has no health left
				if (DamageReceiver->BuildingHP <= 0.0f)
				{
					if (Cast<ACommand_Center>(DamageReceiver))
					{
						if (ARTS_PlayerController* LoserController = Cast<ARTS_PlayerController>(DamageReceiver->OwningController))
						{
							LoserController->ServerPauseGame(true);
							LoserController->ServerAddEndGameUI(false);
						}
						if (ARTS_PlayerController* WinnerController = Cast<ARTS_PlayerController>(DamageOwner->ControlledController))
						{
							WinnerController->ServerPauseGame(true);
							WinnerController->ServerAddEndGameUI(true);
						}
					}
					//we destroy that building
					DamageReceiver->Destroy();
					//and clear the attack timer for the attack unit
					DamageOwner->ClearTimer();
					//We clear the target reference inside of damage owner
					DamageOwner->TargetEnemyRef = nullptr;
				}
			}
			break;
		case Ranger:
			break;
		}
	}
}

void ARTS_PlayerController::ServerSpawnNPCUnitByPlayer_Implementation(TSubclassOf<AActor> Actor,
                                                                      FVector SpawnActorLocation, FRotator Rotation, APlayerController* OwningPlayerController)
{
	if(HasAuthority())
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor(Actor, &SpawnActorLocation,&Rotation);
		if (ANPC* Unit = Cast<ANPC>(SpawnedActor))
		{
			Unit->ControlledController = OwningPlayerController;
		}
	}
}

void ARTS_PlayerController::ServerSpawnNPCUnit_Implementation(TSubclassOf<AActor> Actor, FVector SpawnActorLocation, FRotator Rotation, ABuilding* OwningBuilding)
{
	if(HasAuthority())
	{
		AActor* SpawnedActor = GetWorld()->SpawnActor(Actor, &SpawnActorLocation,&Rotation);
		if (ANPC* Unit = Cast<ANPC>(SpawnedActor))
		{
			Unit->ControlledController = OwningBuilding->OwningController;
		}
	}
}



