// Fill out your copyright notice in the Description page of Project Settings.


#include "Building.h"

#include "InstancedFoliageActor.h"
#include "placableActorComponent.h"
#include "RTSCharacter.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABuilding::ABuilding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//setup the mesh and collision box
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	BuildingMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	BuildingMesh->SetupAttachment(RootComponent);
	BuildCollision=CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Componet"));
	BuildCollision->SetupAttachment(BuildingMesh);
	bReplicates = true;
	BuildingMesh->SetIsReplicated(true);
	BuildCollision->SetIsReplicated(true);

	NetCullDistanceSquared = 224999997440.0;
}

void ABuilding::ChangeColor(bool bIsValid)
{
	if (MaterialInstance && GameState && this)
	{
		//If the building is not overlap with anything
		MulticastChangeColor(bIsValid);
	}
}
void ABuilding::MulticastChangeColor_Implementation(bool bIsValid)
{
	if (bIsValid)
	{
		//We change the color of that material instance to green
		MaterialInstance->SetVectorParameterValue("Color", GameState->ValidColour);
	}
	else
	{
		//if building is overlapping with others, change to red
		MaterialInstance->SetVectorParameterValue("Color", GameState->InValidColour);
	}
}


void ABuilding::ServerSetPlaceableBuildingCollision_Implementation()
{
	MulticastSetPlaceableBuildingCollision();
}

void ABuilding::MulticastSetPlaceableBuildingCollision_Implementation()
{
	this->BuildCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	this->BuildCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	this->BuildCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}


//update the placement status, valid or invalid
void ABuilding::UpdateState()
{
	//Get the placeable actor component
	UplacableActorComponent* ActorComponent = Cast<UplacableActorComponent>(this->GetComponentByClass(UplacableActorComponent::StaticClass()));
	//we did an extra check to see whether we are overlapping or not
	TArray<AActor*> OverlapActors;
	this->GetOverlappingActors(OverlapActors, TSubclassOf<ABuilding>());
	//we use this to check are we only overlap with foliage
	bool bOnlyOverlapWithFoliage = false;
	//if we only got one actor currently overlap with us
	if (OverlapActors.Num() == 1)
	{
		//and it is a instanced foliage actor
		if (Cast<AInstancedFoliageActor>(OverlapActors[0]))
		{
			//then we know we only overlap with the foliage actor
			bOnlyOverlapWithFoliage = true;
		}
	}
	//if the owner is not overlap with any Actors and GetComponents worked or we only overlap with foliage and GetComponents worked
	if (OverlapActors.Num() == 0 && MeshComponents.Num() != 0 || bOnlyOverlapWithFoliage && MeshComponents.Num() != 0)
	{
		//Inform server that we want change to valid color
		if (Cast<ARTS_PlayerController>(OwningController))
		{
			Cast<ARTS_PlayerController>(OwningController)->ServerChangeColor(this, true);
		}
		if (ActorComponent)
		{
			ActorComponent->CanPlop = true;
		}
	}
	//else if the owner is overlapping with other Actors and GetComponents work
	else if (OverlapActors.Num() != 0 && MeshComponents.Num() !=0)
	{
		//Inform server that we want change to invalid color
		//Inform server that we want change to valid color
		if (Cast<ARTS_PlayerController>(OwningController))
		{
			Cast<ARTS_PlayerController>(OwningController)->ServerChangeColor(this, false);
		}
		if (ActorComponent)
		{
			ActorComponent->CanPlop = false;
		}
	}
}

//we use this function to set the material
void ABuilding::ChangeMaterial()
{
	if (!MeshComponents.IsEmpty())
	{
		//loop through all the static mesh components
		for (UStaticMeshComponent* Component: MeshComponents)
		{
			//Cast the ActorMesh to StaticMesh, and make sure cast successful
			if (MaterialInstance)
			{
				//Set the material of static mesh to the material instance that we created in begin play function
				if (Component->GetNumMaterials())
				{
					for (int i = 0; i <= Component->GetNumMaterials(); i++)
					{
						Component->SetMaterial(i,MaterialInstance);
					}
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ABuilding::BeginPlay()
{
	Super::BeginPlay();
	//Get the customized game state that we created
	GameState = Cast<ARTSGameState>(GetWorld()->GetGameState());
	//use this to get all the static mesh components of current building
	//but they are UActorComponent for now, we need cast them later
	TArray<UActorComponent*> Components;
	this->GetComponents(UStaticMeshComponent::StaticClass(),Components);
	//if we get the static mesh component successfully
	if (!Components.IsEmpty())
	{
		//loop through the Components that we get before
		for (auto const Component:Components)
		{
			//Cast them into UStaticMeshComponent and store in the MeshComponents global variable
			UStaticMeshComponent* MeshComponent = Cast<UStaticMeshComponent>(Component);
			MeshComponents.Add(MeshComponent);
		}
	}
	if (MaterialClass)
	{
		//Use this to create a dynamic material instance
		MaterialInstance = UMaterialInstanceDynamic::Create(MaterialClass, this);
	}
}

// Called every frame
void ABuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//These 3 functions will be override in child class
void ABuilding::Function()
{
}

void ABuilding::SetSelection()
{
}

void ABuilding::SetDeselection()
{
}

//We need to override this function in the child class to make sure this will work on the client side
void ABuilding::SetLocation(const FVector Location)
{
	
}

void ABuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABuilding, OwningController);
	DOREPLIFETIME(ABuilding, BuildingHP);
}

AController* ABuilding::GetCurrentLocalController()
{
	return GetWorld()->GetFirstPlayerController();
}

void ABuilding::SetOwningController(AController* Controller)
{
	OwningController = Controller;
}




