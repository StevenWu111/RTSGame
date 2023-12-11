// Fill out your copyright notice in the Description page of Project Settings.


#include "Resource_Center.h"
#include "RTSCharacter.h"
#include "RTS_PlayerController.h"
#include "Blueprint/UserWidget.h"


void AResource_Center::BeginPlay()
{
	Super::BeginPlay();
	BuildCollision->OnComponentBeginOverlap.AddDynamic(this, &AResource_Center::OnBeginOverlap);
}

AResource_Center::AResource_Center()
{
	WoodCost=Wood;
	MetalCost=Metal;
	bReplicates = true;
	BuildingMesh->SetIsReplicated(true);
	NetPriority = 3.0f;
}

void AResource_Center::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void AResource_Center::SetSelection()
{
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

void AResource_Center::SetDeselection()
{
	if (WidgetInstance)
	{
		WidgetInstance->RemoveFromParent();
	}
}

void AResource_Center::Function()
{
	Super::Function();
	//UE_LOG(LogTemp,Warning,TEXT("Server try to Spawn Actor"))
	//chose a random location to spawn the pawn
	FVector SpawnLocation(FMath::RandRange(-3000.0f, -1200.0f),FMath::RandRange(-1200.0f, -3000.0f),0);
	SpawnLocation += this->GetActorLocation();
	//if we have the worker class (UPROPERTY(EditAnywhere)) and a controller that owning this building
	if (ARTSCharacter* Player = Cast<ARTSCharacter>(OwningController->GetPawn()))
	{
		if (Player->PlayerState->WoodCount >= WorkerOilCost && Player->PlayerState->MetalCount >= WorkerMetalCost)
		{
			if (Worker && OwningController)
			{
				//We call the ServerSpawnNPCUnit from the our player controller class
				//GetWorld()->SpawnActor(Worker, &SpawnLocation,&FRotator::ZeroRotator);
				Cast<ARTS_PlayerController>(OwningController)->ServerSpawnNPCUnit(Worker, SpawnLocation, FRotator::ZeroRotator, this);
				Player->ServerCalculateCost(-WorkerMetalCost, -WorkerOilCost);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("Your resources is not enough for Worker, require 30 oil and 70 metal"));
		}
	}
}

void AResource_Center::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (WidgetInstance)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Has Widget Instance"))
		if (WidgetInstance->StartGenerateWorker)
		{
			Function();
			WidgetInstance->StartGenerateWorker=false;
		}
		
	}
}

void AResource_Center::SetLocation(const FVector Location)
{
	Super::SetLocation(Location);
	this->SetActorLocation(Location);
}


