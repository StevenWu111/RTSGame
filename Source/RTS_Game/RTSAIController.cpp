// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSAIController.h"

#include "NPC.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

ARTSAIController::ARTSAIController()
{
}

void ARTSAIController::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void ARTSAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANPC* Unit = Cast<ANPC>(InPawn))
	{
		//UE_LOG(LogTemp,Warning,TEXT("Controller possessed"))
		//Unit->AttackRange->OnComponentBeginOverlap.AddDynamic(this, &ARTSAIController::OnOverlapBegin);
		
	}
}

void ARTSAIController::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap begin"))
}

void ARTSAIController::MoveToTargetLocation(FVector Location)
{
	//Make sure they are not moving in Z direction
	//Location.Z = GetPawn()->GetActorLocation().Z;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Location);
}
