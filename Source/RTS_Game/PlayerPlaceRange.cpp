// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPlaceRange.h"

// Sets default values
APlayerPlaceRange::APlayerPlaceRange()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	Range = CreateDefaultSubobject<USphereComponent>(TEXT("Place Range"));
	Range->SetupAttachment(RootComponent);
	Range->SetSphereRadius(100);
}

// Called when the game starts or when spawned
void APlayerPlaceRange::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void APlayerPlaceRange::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

