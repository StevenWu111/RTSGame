// Fill out your copyright notice in the Description page of Project Settings.


#include "WorkerCharacter.h"
#include "InstancedFoliageActor.h"
#include "Resource_Center.h"
#include "RTS_PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Multi_PlayerState.h"
#include "Net/UnrealNetwork.h"

AWorkerCharacter::AWorkerCharacter()
{
	AgentType = Melee;
}

void AWorkerCharacter::BeginPlay()
{
	Super::BeginPlay();
	AttackRange->SetSphereRadius(50);
	AttackRange->OnComponentBeginOverlap.AddDynamic(this,&AWorkerCharacter::OnOverlapBegin);
	AttackRange->OnComponentEndOverlap.AddDynamic(this,&AWorkerCharacter::OnOverlapOver);
}

void AWorkerCharacter::Destination()
{
	Super::Destination();
}

void AWorkerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//check our target enemy is still valid or not
	if (TargetEnemyRef && !TargetEnemyRef->IsValidLowLevelFast())
	{
		//if it is not valid for some reason, we clear the TargetEnemyRef pointer
		TargetEnemyRef = nullptr;
	}

	if (TargetResourcesPoint && !TargetResourcesPoint->IsValidLowLevelFast())
	{
		TargetResourcesPoint = nullptr;
	}
	if (WorkerBackTargetBuilding && !WorkerBackTargetBuilding->IsValidLowLevelFast())
	{
		WorkerBackTargetBuilding = nullptr;
		MoveToTargetLocation(this->GetActorLocation());
	}
	if(!TargetEnemyRef && HasAuthority())
	{
		ClearTimer();
		//if we allow unit to attack freely
		if(bAttackFreely)
		{
			//there are some enemy in our range
			if (!AttackUnitList.IsEmpty())
			{
				//we loop through it
				for (auto CurrTarget:AttackUnitList)
				{
					//we check is it is valid
					if (CurrTarget->IsValidLowLevel())
					{
						//UE_LOG(LogTemp, Warning, TEXT("%s start attack"), *this->GetName());
						//if valid, we attack it and break the loop
						this->AttackTarget(CurrTarget);
						break;
					}
					else
					{
						//if not valid, we remove it from the list
						AttackUnitList.Remove(CurrTarget);
					}
				}
			}
			else if (!AttackBuildingList.IsEmpty())
			{
				if (AttackBuildingList[0] && AttackBuildingList[0]->IsValidLowLevel())
				{
					this->AttackTarget(AttackBuildingList[0]);
				}
			}
		}
	}
}

void AWorkerCharacter::SetSelection()
{
	Super::SetSelection();
	if (Widget)
	{
		WidgetInstance=CreateWidget<UUserWidget>(GetWorld(), Widget);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
		}
	}

	//GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("Select"));
}

void AWorkerCharacter::SetDeselected()
{
	Super::SetDeselected();
	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromParent();
	}
	//GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("Cancel"));
}

// called when overlap start
void AWorkerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!Cast<AInstancedFoliageActor>(OtherActor) && HasAuthority())
	{
		//if the overlap actor is a building
		if (ABuilding* CurrBuilding = Cast<ABuilding>(OtherActor))
		{
			//if it belongs to the enemy
			if (CurrBuilding->OwningController != ControlledController && !CurrBuilding->bIsPlacing)
			{
				//add to out attackable building array
				AttackBuildingList.Add(CurrBuilding);
			}
		}
		//if it is a unit
		else if (ANPC* CurrUnit = Cast<ANPC>(OtherActor))
		{
			//and it belongs to the enemy
			if (CurrUnit->ControlledController != ControlledController)
			{
				//add to out attackable unit array
				AttackUnitList.Add(CurrUnit);
			}
		}
		//if it is a resources point
		else if(AResourcesPoint* CurrResourcesPoint = Cast<AResourcesPoint>(OtherActor))
		{
			//we add it into out InRangeResourcesPoints array
			InRangeResourcesPoints.Add(CurrResourcesPoint);
		}
		//UE_LOG(LogTemp, Warning, TEXT("Unit overlap happened"))
		//if the overlap actor is the target that we trying to attack
		if (OtherActor == TargetEnemyRef && AttackFrequency)
		{//UE_LOG(LogTemp,Warning,TEXT("set the attack timer"))
			//start the timer to attack
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWorkerCharacter::MakeServerCalculateDamage, AttackFrequency, true, 0.0f);
		}
		else if (OtherActor == TargetResourcesPoint)
		{
			if (CurrCarryingAmount <= MaxGather && CurrResourcesType == TargetResourcesPoint->ResourcesType)
			{
				GetWorld()->GetTimerManager().SetTimer(GatheringTimerHandle, this, &AWorkerCharacter::MakerServerAddResourcesToWorker, GatheringSpeed, true, 0.0f);
			}
		}


	
		/*
		 *
		 * When overlap with resources center
		 * 
		 */
		//UE_LOG(LogTemp,Warning,TEXT("Overlap Happened"));
		if (OtherActor && OtherActor!=this)
		{
			const AResource_Center* OverlappedResource_Center = Cast<AResource_Center>(OtherActor);
			//we check if it is a resources center or not
			if (OverlappedResource_Center->IsValidLowLevel() && !OverlappedResource_Center->bIsPlacing)
			{
				//if it is valid we stop moving
				MoveToTargetLocation(this->GetActorLocation());
				//then we clear the carrying amount from server and add the carrying amount to player
				if (ARTS_PlayerController* PlayerController = Cast<ARTS_PlayerController>(ControlledController))
				{
					if (AMulti_PlayerState* CurrPlayerState = Cast<AMulti_PlayerState>(PlayerController->GetPawn()->GetPlayerState()))
					{
						PlayerController->ServerClearTheCarryingAmount(this, CurrPlayerState);
					}
				}
				//if the target resources point is still valid, we move to there again
				if (this->TargetResourcesPoint)
				{
					this->MoveToTargetLocation(TargetResourcesPoint->GetActorLocation());
				}
			}
		}
	}
}

//called when overlap end
void AWorkerCharacter::OnOverlapOver(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		//we check any of them are valid
		ABuilding* CurrBuilding = Cast<ABuilding>(OtherActor);
		ANPC* CurrUnit = Cast<ANPC>(OtherActor);
		AResourcesPoint* CurrResourcesPoint = Cast<AResourcesPoint>(OtherActor);
		//if it is a building and already in the list, we remove it from the list, which means its not longer in range
		if (CurrBuilding && AttackBuildingList.Contains(OtherActor))
		{
			AttackBuildingList.Remove(CurrBuilding);
		}
		//if it is a Unit and already in the list, we remove it from the list, which means its not longer in range
		else if (CurrUnit && AttackUnitList.Contains(OtherActor))
		{
			AttackUnitList.Remove(CurrUnit);
		}
		///if it is a resources point and already in the list, we remove it from the list, which means its not longer in range
		else if (CurrResourcesPoint && InRangeResourcesPoints.Contains(CurrResourcesPoint))
		{
			InRangeResourcesPoints.Remove(CurrResourcesPoint);
		}
		if (OtherActor == TargetEnemyRef)
		{
			if (bChase)
			{
				AttackTarget(TargetEnemyRef);
			}
			else
			{
				TargetEnemyRef = nullptr;
			}
		}
		else if (OtherActor == TargetResourcesPoint)
		{
			Cast<ARTS_PlayerController>(ControlledController)->ServerClearTimer(GatheringTimerHandle);
		}
	}
}

//Called when we want to make it collect the resources
void AWorkerCharacter::CollectResources(AResourcesPoint* ResourcesPoint, TArray<ABuilding*> PlayerSpawnedBuildings)
{
	if (CurrCarryingAmount <= MaxGather)
	{
		if (CurrCarryingAmount == 0.0f && CurrResourcesType == None)
		{
			CurrResourcesType = ResourcesPoint->ResourcesType;
		}
		if (CurrResourcesType != None && CurrResourcesType == ResourcesPoint->ResourcesType)
		{
			//Update the target to the variable in this class
			TargetResourcesPoint = ResourcesPoint;
			//Check to see if it is valid or not
			if (TargetResourcesPoint && TargetResourcesPoint->IsValidLowLevel())
			{
				//if the target is currently in our range
				if (InRangeResourcesPoints.Contains(TargetResourcesPoint))
				{
					//we allow the worker to start calculate resources
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AWorkerCharacter::MakerServerAddResourcesToWorker, GatheringSpeed, true, GatheringSpeed);
				}
				else
				{
					//if the target resources point is not in range, we need move toward it.
					MoveToTargetLocation(TargetResourcesPoint->GetActorLocation());
				}
				float CurrMinDis = INFINITY;
				int32 IndexTemp = -1;
				for (int i =0; i < PlayerSpawnedBuildings.Num(); i++)
				{
					if (Cast<AResource_Center>(PlayerSpawnedBuildings[i]))
					{
						if (FVector::Distance(PlayerSpawnedBuildings[i]->GetActorLocation(), TargetResourcesPoint->GetActorLocation()) <= CurrMinDis)
						{
							CurrMinDis = FVector::Distance(PlayerSpawnedBuildings[i]->GetActorLocation(), TargetResourcesPoint->GetActorLocation());
							IndexTemp = i;
						}
					}
				}
				if (IndexTemp != -1)
				{
					WorkerBackTargetBuilding = PlayerSpawnedBuildings[IndexTemp];
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("You are carrying other type of resources"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("You cannot carry more resources"));
	}
}

void AWorkerCharacter::MakerServerAddResourcesToWorker()
{
	MoveToTargetLocation(this->GetActorLocation());
	FVector BackLocation;
	if (WorkerBackTargetBuilding)
	{
		BackLocation = WorkerBackTargetBuilding->GetActorLocation() + FVector(1000,0,0);
	}
	if (ARTS_PlayerController* PlayerController = Cast<ARTS_PlayerController>(ControlledController))
	{
		PlayerController->ServerAddResourcesToWorker(this, BackLocation);
	}
}

void AWorkerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorkerCharacter, GatheringSpeed);
	DOREPLIFETIME(AWorkerCharacter, CurrCarryingAmount);
	DOREPLIFETIME(AWorkerCharacter, MaxGather);
	DOREPLIFETIME(AWorkerCharacter, TargetResourcesPoint);
}
