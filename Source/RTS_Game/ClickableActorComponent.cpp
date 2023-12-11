// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickableActorComponent.h"

#include "RTSCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UClickableActorComponent::UClickableActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UClickableActorComponent::BeginPlay()
{
	Super::BeginPlay();
	//Only valid when the owner is a building
	CurrentBuilding = Cast<ABuilding>(GetOwner());
	//Only valid when the owner is a Unit
	CurrentUnit = Cast<ANPC>(GetOwner());

	CurrWoodResource = Cast<AOilResourcesPoint>(GetOwner());
	CurrMetalResource = Cast<AMetalResourcesPoint>(GetOwner());
	UE_LOG(LogTemp,Warning, TEXT("Can Click"))
	//Check if the owner is building or not
	if (CurrentBuilding)
	{
		//we only want on click event working on the building, any other actor like the units will be select by the HUD
		CurrentBuilding->OnClicked.AddDynamic(this,&UClickableActorComponent::OnMouseClicked);
		//Get the static mesh component of current owner building
		CurrentBuilding->GetComponents(UStaticMeshComponent::StaticClass(), MeshComponents);
		//UE_LOG(LogTemp,Warning, TEXT("This actor has component with length = %d"), MeshComponents.Num());
	}
	if (CurrentUnit)
	{
		//Get the static mesh component of current owner building
		CurrentUnit->GetComponents(UStaticMeshComponent::StaticClass(), MeshComponents);
	}
	if (CurrWoodResource)
	{
		CurrWoodResource->GetComponents(UStaticMeshComponent::StaticClass(), MeshComponents);
	}
	if (CurrMetalResource)
	{
		CurrMetalResource->GetComponents(UStaticMeshComponent::StaticClass(), MeshComponents);
	}
	//we want all the actors with this actor component can have the selection shader on when the mouse hover over
	//So we bind events with OnBeginCursorOver and OnEndCursorOver
	GetOwner()->OnBeginCursorOver.AddDynamic(this, &UClickableActorComponent::OnMouseHover);
	GetOwner()->OnEndCursorOver.AddDynamic(this, &UClickableActorComponent::OnMouseHoverOver);
}

//Again, this function will only be called when owner is a building
void UClickableActorComponent::OnMouseClicked(AActor* TouchedActor, FKey ButtonPressed)
{
	//once player clicked it, set IsClicked = true
	IsClicked = true;
	//Double check the Current building variable is valid or not
	if (CurrentBuilding)
	{
		//Get the player who clicked
		ARTSCharacter* Player = Cast<ARTSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		//we got the player who clicked on this and if this building belongs to this player or not (CanClick or not)
		if (Player && bIsBuildingCanClick)
		{
			//Call the set selection function inside of building, which add the UI to the player screen
			CurrentBuilding->SetSelection();
			//Update the selected building to player class
			Player->SelectedBuilding = CurrentBuilding;
			//turn on the selection shader (this will turn off in the player class)
			ChangeCustomDepth(true, 10);
			//UE_LOG(LogTemp,Warning, TEXT("UI Added"))
		}
		else if (Player)
		{
			//Update the selected building to player class
			Player->SelectedBuilding = CurrentBuilding;
		}
	}
	//UE_LOG(LogTemp,Warning, TEXT("Selected"))
}

//when player hover this actor, turn on the selection shader
void UClickableActorComponent::OnMouseHover(AActor* TouchedActor)
{
	//UE_LOG(LogTemp,Warning, TEXT("Hover"))
	//Make sure our owner is a building
	if (CurrentBuilding)
	{
		//UE_LOG(LogTemp,Warning, TEXT("Has owning building"))
		//Make sure we got a player state from Building class
		if (CurrentBuilding->GetCurrentLocalController())
		{	
			//UE_LOG(LogTemp,Warning,TEXT("Got Local Player"))
			//Make sure our owning building has a owner(Player state) which is a player
			//if (CurrentBuilding->OwningController)
			//{
				//UE_LOG(LogTemp, Warning, TEXT("This Building Has Player State"))
				//When the owner of this building is same as the current controlled building, BLUE shader display
				if (CurrentBuilding->OwningController == CurrentBuilding->GetCurrentLocalController())
				{
					//Set StencilValue to 10 means we change the selection shader to blue (This building to belong to you)
					ChangeCustomDepth(true, 10);
					bIsBuildingCanClick = true;
				}
				//When the owner of this building is not the current controlled player, RED selection shader display
				else
				{
					//Set to 0 means we change it to red (This Building is not belong to you)
					ChangeCustomDepth(true, 0);
					bIsBuildingCanClick = false;
					//If the target building belongs to enemy player, we update the texture of mouse cursor to attack mode
					if (Cast<APlayerController>(CurrentBuilding->GetCurrentLocalController()))
					{
						Cast<APlayerController>(CurrentBuilding->GetCurrentLocalController())->CurrentMouseCursor = EMouseCursor::TextEditBeam;
					}
				}
			//}
		}
	}

	if (CurrentUnit)
	{
		if (CurrentUnit->GetCurrentLocalController())
		{
			if (CurrentUnit->ControlledController == CurrentUnit->GetCurrentLocalController())
			{
				//Set StencilValue to 10 means we change the selection shader to blue (This building to belong to you)
				ChangeCustomDepth(true, 10);
			}
			//When the owner of this building is not the current controlled player, RED selection shader display
			else
			{
				//Set to 0 means we change it to red (This Building is not belong to you)
				ChangeCustomDepth(true, -10);
				//If the target building belongs to enemy player, we update the texture of mouse cursor to attack mode
				if (Cast<APlayerController>(CurrentUnit->GetCurrentLocalController()))
				{
					Cast<APlayerController>(CurrentUnit->GetCurrentLocalController())->CurrentMouseCursor = EMouseCursor::TextEditBeam;
				}
			}
		}
	}

	if (CurrWoodResource || CurrMetalResource)
	{
		if (APlayerController* FirstPlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			FirstPlayerController->CurrentMouseCursor = EMouseCursor::ResizeLeftRight;
			ChangeCustomDepth(true, 10);
		}
	}
}

//When player hover over, turn off the selection shader
void UClickableActorComponent::OnMouseHoverOver(AActor* TouchActor)
{
	//we want clicked actor keep the selection shader.
	//only the non selected actor will turn off the shader
	if (!IsClicked && CurrentBuilding)
	{
		ChangeCustomDepth(false,0);
		//when the overlap end, we set the mouse cursor to defualt
		if (Cast<APlayerController>(CurrentBuilding->GetCurrentLocalController()))
		{
			Cast<APlayerController>(CurrentBuilding->GetCurrentLocalController())->CurrentMouseCursor = EMouseCursor::Default;
		}
	}else if (CurrentUnit)
	{
		if (!CurrentUnit->bIsSelect)
		{
			ChangeCustomDepth(false, 0);
		}
		if (Cast<APlayerController>(CurrentUnit->GetCurrentLocalController()))
		{
			Cast<APlayerController>(CurrentUnit->GetCurrentLocalController())->CurrentMouseCursor = EMouseCursor::Default;
		}
	}
	else if (CurrWoodResource || CurrMetalResource)
	{
		ChangeCustomDepth(false, 0);
		if (APlayerController* FirstPlayerController = Cast<APlayerController>(GetWorld()->GetFirstPlayerController()))
		{
			FirstPlayerController->CurrentMouseCursor = EMouseCursor::Default;
		}
	}
}

//use this to turn on/off the selection shader
void UClickableActorComponent::ChangeCustomDepth(bool ActiveSelectionShader, int StencilValue)
{
	if (MeshComponents.Num() != 0)
	{
		//loop through all the actor component that we got before and cast them into static mesh component
		//Then set the RenderCustomDepth to true/false (Turn on/off)
		for (auto Component: MeshComponents)
		{
			if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Component))
			{
				StaticMesh->SetRenderCustomDepth(ActiveSelectionShader);
				if (StencilValue)
				{
					StaticMesh->SetCustomDepthStencilValue(StencilValue);
				}
			}
		}
	}
}


// Called every frame
void UClickableActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

