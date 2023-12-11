// Fill out your copyright notice in the Description page of Project Settings.


#include "Arsenal.h"

#include "Building_Widget.h"
#include "RTSCharacter.h"
#include "RTS_PlayerController.h"


AArsenal::AArsenal()
{
	WoodCost = Wood;
	MetalCost = Metal;
	bReplicates = true;
	BuildingMesh->SetIsReplicated(true);
	NetPriority = 3.0f;
}

void AArsenal::BeginPlay()
{
	Super::BeginPlay();
	BuildCollision->OnComponentBeginOverlap.AddDynamic(this, &AArsenal::OnBeginOverlap);
}

void AArsenal::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AArsenal::SetLocation(const FVector Location)
{
	Super::SetLocation(Location);
	this->SetActorLocation(Location);
}

void AArsenal::Function()
{
	Super::Function();

	FVector SpawnLocation(FMath::RandRange(-3000.0f, -1200.0f),FMath::RandRange(-1200.0f, -3000.0f),0);
	SpawnLocation += this->GetActorLocation();

	if (ARTSCharacter* Player = Cast<ARTSCharacter>(OwningController->GetPawn()))
	{
		if (Player->PlayerState->WoodCount >= SoldierOilCost && Player->PlayerState->MetalCount >= SoldierMetalCost)
		{
			if (Solider && OwningController)
			{
				//We call the ServerSpawnNPCUnit from the our player controller class
				//GetWorld()->SpawnActor(Worker, &SpawnLocation,&FRotator::ZeroRotator);
				Cast<ARTS_PlayerController>(OwningController)->ServerSpawnNPCUnit(Solider, SpawnLocation, FRotator::ZeroRotator, this);
				Player->ServerCalculateCost(-SoldierMetalCost, -SoldierOilCost);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("Your resources is not enough for soldier, require 80 oil and 50 metal"));
		}
	}
}

void AArsenal::SetSelection()
{
	Super::SetSelection();

	if (Widget)
	{
		WidgetInstance=CreateWidget<UBuilding_Widget>(GetWorld(), Widget);
		if (WidgetInstance)
		{
			WidgetInstance->AddToViewport();
			WidgetInstance->Building=this;
		}
	}
	
}

void AArsenal::SetDeselection()
{
	Super::SetDeselection();
	
	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromParent();
		
	}
	
	
}

void AArsenal::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WidgetInstance)
	{
		if (WidgetInstance->StartGenerateSolider)
		{
			Function();
			WidgetInstance->StartGenerateSolider=false;
		}
		
	}
}
