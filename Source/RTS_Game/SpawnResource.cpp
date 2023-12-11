// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnResource.h"

// Sets default values
ASpawnResource::ASpawnResource()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnResource::BeginPlay()
{
	Super::BeginPlay();
	
	for(int i = 0; i < MaxTree; i++)
	{
		FVector SpawnLocation(FMath::RandRange(0.0f, 10000.0f),FMath::RandRange(0.0f, 10000.0f),220.0f);
		if(TreeClass)
		{
			AWood_Resource* Wood=Cast<AWood_Resource>(GetWorld()->SpawnActor(TreeClass,&SpawnLocation));
			if (Wood)
			{
				//random size
				Wood->SetActorScale3D(Wood->GetActorScale()* FMath::RandRange(0.5f,0.8f));
			}
			
		}
	}

	for(int i = 0; i < MaxMetal; i++)
	{
		FVector SpawnLocation(FMath::RandRange(0.0f, 10000.0f),FMath::RandRange(0.0f, 10000.0f),100.0f);
		if(MetalClass)
		{
			AMetal_Resource* Metal=Cast<AMetal_Resource>(GetWorld()->SpawnActor(MetalClass,&SpawnLocation));
			if (Metal)
			{
				//random size
				Metal->SetActorScale3D(Metal->GetActorScale()* FMath::RandRange(0.3f,0.6f));
			}
		}
	}
	
}

// Called every frame
void ASpawnResource::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(NumTree < MaxTree * 0.2f)
	{
		while(NumTree< MaxTree)
		{
			SpawnTree();
		}
	}
	if(NumMetal < MaxMetal * 0.2f)
	{
		while(NumMetal< MaxMetal)
		{
			SpawnMetal();
		}
	}


}

void ASpawnResource::SpawnTree()
{
	
	FVector SpawnLocation(FMath::RandRange(0.0f, 10000.0f),FMath::RandRange(0.0f, 10000.0f),220.0f);
	if(TreeClass)
	{
		AWood_Resource* Spawned = Cast<AWood_Resource>(GetWorld()->SpawnActor(TreeClass, &SpawnLocation,&FRotator::ZeroRotator));
		if(Spawned)
		{
			NumTree++;
		}
	
	}
	
}

void ASpawnResource::SpawnMetal()
{
	
	FVector SpawnLocation(FMath::RandRange(0.0f, 10000.0f),FMath::RandRange(0.0f, 10000.0f),100.0f);
	if(MetalClass)
	{
		AMetal_Resource* Spawned = Cast<AMetal_Resource>(GetWorld()->SpawnActor(MetalClass, &SpawnLocation,&FRotator::ZeroRotator));
		if(Spawned)
		{
			NumMetal++;
		}
	
	}
	
}




