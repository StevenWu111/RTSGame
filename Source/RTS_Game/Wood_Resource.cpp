// Fill out your copyright notice in the Description page of Project Settings.


#include "Wood_Resource.h"

#include "WorkerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AWood_Resource::AWood_Resource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	
	Treemesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	Treemesh->SetupAttachment(RootComponent);

	collisionbox=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Componet"));
	collisionbox->SetupAttachment(Treemesh);




}


// Called when the game starts or when spawned
void AWood_Resource::BeginPlay()
{
	Super::BeginPlay();

	collisionbox->OnComponentBeginOverlap.AddDynamic(this,&AWood_Resource::OnBeginOverlap);
	collisionbox->OnComponentEndOverlap.AddDynamic(this,&AWood_Resource::OnEndOverlap);
	
	
}


void AWood_Resource::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AWood_Resource::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


// Called every frame
void AWood_Resource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);



}


