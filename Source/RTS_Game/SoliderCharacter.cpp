// Fill out your copyright notice in the Description page of Project Settings.


#include "SoliderCharacter.h"

#include "RTS_PlayerController.h"

ASoliderCharacter::ASoliderCharacter()
{
	AgentType = Ranger;
}

void ASoliderCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}

void ASoliderCharacter::Destination()
{
	Super::Destination();
}

void ASoliderCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	//check our target enemy is still valid or not
	if (TargetEnemyRef && !TargetEnemyRef->IsValidLowLevel())
	{
		//if it is not valid for some reason, we clear the TargetEnemyRef pointer
		TargetEnemyRef = nullptr;;
	}
	if (!TargetEnemyRef && ControlledController)
	{
		Cast<ARTS_PlayerController>(ControlledController)->ServerClearTimer(TimerHandle);
	}
}



void ASoliderCharacter::SetSelection()
{
	Super::SetSelection();
}

void ASoliderCharacter::SetDeselected()
{
	Super::SetDeselected();
}

void ASoliderCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ASoliderCharacter::OnOverlapOver(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
