// Fill out your copyright notice in the Description page of Project Settings.


#include "placableActorComponent.h"

#include "Building.h"

// Sets default values for this component's properties
UplacableActorComponent::UplacableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	if (GetOwner())
	{
		if (Cast<ABuilding>(GetOwner()))
		{
			Cast<ABuilding>(GetOwner())->BuildingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Vehicle,ECR_Ignore);
		}
		
		//PloppableMaterial = Cast<ABuilding>(GetOwner())->ValidMaterial;
		//InValidMaterial = Cast<ABuilding>(GetOwner())->InValidMaterial;
	}
	// ...
}


// Called when the game starts
void UplacableActorComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner())
	{
		OwnerBuilding = Cast<ABuilding>(GetOwner());
		GetOwner()->OnActorBeginOverlap.AddDynamic(this, &UplacableActorComponent::OnOverlapBegin);
		GetOwner()->OnActorEndOverlap.AddDynamic(this, &UplacableActorComponent::OnOverlapEnd);
		//Change the material based on the dynamic material instance
		OwnerBuilding->ChangeMaterial();
		OwnerBuilding->UpdateState();
	}
}

void UplacableActorComponent::OnOverlapBegin(AActor* Owner, AActor* OtherActor)
{
	if (OwnerBuilding)
	{
		UE_LOG(LogTemp,Warning,TEXT("Update color"))
		OwnerBuilding->UpdateState();
	}
}

void UplacableActorComponent::OnOverlapEnd(AActor* Owner, AActor* OtherActor)
{
	if (OwnerBuilding)
	{
		UE_LOG(LogTemp,Warning,TEXT("Update color"))
		OwnerBuilding->UpdateState();
	}
}


// Called every frame
void UplacableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//void UplacableActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
	//Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UplacableActorComponent, PloppableMaterial);
	//DOREPLIFETIME(UplacableActorComponent, InValidMaterial);

//}

