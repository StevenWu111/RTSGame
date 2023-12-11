// Fill out your copyright notice in the Description page of Project Settings.


#include "Metal_Resource.h"

// Sets default values
AMetal_Resource::AMetal_Resource()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Mesh->SetupAttachment(RootComponent);

	collisionbox=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Componet"));
	collisionbox->SetupAttachment(Mesh);




}


// Called when the game starts or when spawned
void AMetal_Resource::BeginPlay()
{
	Super::BeginPlay();

	collisionbox->OnComponentBeginOverlap.AddDynamic(this,&AMetal_Resource::OnBeginOverlap);
	collisionbox->OnComponentEndOverlap.AddDynamic(this,&AMetal_Resource::OnEndOverlap);
	
	
}


void AMetal_Resource::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AMetal_Resource::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


// Called every frame
void AMetal_Resource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

