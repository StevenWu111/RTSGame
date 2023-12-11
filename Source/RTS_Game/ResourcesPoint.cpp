// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourcesPoint.h"

// Sets default values
AResourcesPoint::AResourcesPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	StaticMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	StaticMesh->SetupAttachment(RootComponent);

	CollisionBox=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Componet"));
	CollisionBox->SetupAttachment(StaticMesh);

	bReplicates = true;
	StaticMesh->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AResourcesPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AResourcesPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

