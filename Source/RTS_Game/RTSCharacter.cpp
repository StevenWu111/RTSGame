// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCharacter.h"
#include "ClickableActorComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "FoliageInstancedStaticMeshComponent.h"
#include "Multi_PlayerState.h"
#include "NiagaraFunctionLibrary.h"
#include "placableActorComponent.h"
#include "WorkerCharacter.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ARTSCharacter::ARTSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	//Create Spring Arm Component
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	//Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	//Set everything attacked
	CameraArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(CameraArm,USpringArmComponent::SocketName);
	//initialize the the default camera arm data
	CameraArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f),FRotator(0.0f, 0.0f, 0.0f));
	CameraArm->TargetArmLength = 500.f;
	CameraArm->bEnableCameraLag = true;
	CameraArm->CameraLagSpeed = 3.0f;
	//Adjust these value based on the data that we give in the editor (UPROPERTY(EditAnywhere))
	GetCharacterMovement()->MaxAcceleration  = AccSpeed;
	GetCharacterMovement() -> MaxWalkSpeed = MoveSpeed;
}

//Called when the game starts or when spawned
void ARTSCharacter::BeginPlay()
{
	Super::BeginPlay();
	//Get the player state and cast to out customized state
	PlayerState = Cast<AMulti_PlayerState>(this->GetPlayerState());
	//Get the game state when the game begin
	GameState = Cast<ARTSGameState>(GetWorld()->GetGameState());
	//Spawn the initial worker for the character
	//Make sure this is controlled by a player controller
	PlayerController = Cast<ARTS_PlayerController>(Controller);
	if (PlayerController)
	{
		//this is the requirement for the enhanced input system, which can bind the MappingContext with the local player subsystem
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
		//Get the viewport size of the player
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		Hud = Cast<ARTS_Hud>(PlayerController->GetHUD());
		//We need give player some initial units to allow them start their game
		for (auto Unit:InitialUnits)
		{
			float X = GetActorLocation().X;
			float Y = GetActorLocation().Y;
			FVector SpawnLocation(FMath::RandRange(X + 100, X - 100),FMath::RandRange(Y + 100, Y - 100),510);
			PlayerController->ServerSpawnNPCUnitByPlayer(Unit, SpawnLocation, FRotator::ZeroRotator, PlayerController);
		}
		if (CommandCenter)
		{
			FVector CommandCenterSpawnLocation = this->GetActorLocation() - FVector(0,2000,0);
			CommandCenterSpawnLocation.Z = 140.0f;
			ServerSpawnBuilding(CommandCenter, CommandCenterSpawnLocation, FRotator::ZeroRotator);
		}
		if (PlaceRangeClass)
		{
			//ServerSpawnPlaceRange();
		}
		ClientCreateUI();
	}
}

void ARTSCharacter::ServerSpawnActor_Implementation(TSubclassOf<AActor> SpawnActor, FVector SpawnLocation, FRotator SpawnRotation)
{
	AActor* Temp = GetWorld()->SpawnActor(SpawnActor, &SpawnLocation, &SpawnRotation);
	if (Cast<APlayerPlaceRange>(Temp))
	{
		PlaceRange = Cast<APlayerPlaceRange>(Temp);
	}
}

//WS to control the player move forward and move backward
void ARTSCharacter::Move(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		FVector Forward = this->GetActorForwardVector();
		Forward.Z = 0;
		Forward.Normalize();
		AddMovementInput(Forward, DirectionValue);
		//UE_LOG(LogTemp, Warning, TEXT("X = %f"), Forward.X);
		//UE_LOG(LogTemp, Warning, TEXT("Y = %f"), Forward.Y);
		//UE_LOG(LogTemp, Warning, TEXT("Z = %f"), Forward.Z);
	}
}

//AD to control the player move horizontally
void ARTSCharacter::MoveAD(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		FVector Right = this->GetActorRightVector();
		AddMovementInput(Right, DirectionValue);
	}
}

//Allow player to free look when they holding right button and move the mouse
void ARTSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	if (GetController() && bIsMidButtonHold)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Rotating"));
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
		int X = 0;
		int Y = 0;
		GetWorld()->GetFirstPlayerController()->GetViewportSize(X, Y);
		if (ViewportSize != FVector2d(0,0))
		{
			GetWorld()->GetFirstPlayerController()->SetMouseLocation(X/2, Y/2);
		}
	}
}
//Zoom out
void ARTSCharacter::MoveUp(const FInputActionValue& Value)
{
	CameraZoom(Value);
}
//Rotate left and right only by the QE keys
void ARTSCharacter::Rotate(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		AddControllerYawInput(DirectionValue);
	}
}

//When player holding mid button, we allow it to rotate freely but disable the functionality of mouse cursor
void ARTSCharacter::HoldingRight(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Right mouse button triggered"));
	bIsMidButtonHold = true;
	//GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	GetWorld()->GetFirstPlayerController()->bEnableTouchEvents = false;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = false;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = false;
}

//When player release, we disable free look and bring the functions back to the mouse cursor
void ARTSCharacter::ReleaseRight(const FInputActionValue& Value)
{
	//UE_LOG(LogTemp, Warning, TEXT("Released"));
	bIsMidButtonHold = false;
	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
	GetWorld()->GetFirstPlayerController()->bEnableTouchEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableMouseOverEvents = true;
	GetWorld()->GetFirstPlayerController()->bEnableClickEvents = true;
}

//Zoom with wheel
void ARTSCharacter::CameraZoom(const FInputActionValue& Value)
{
	const float ValueAmount = 0 - Value.Get<float>();
	if (CameraArm && GetController())
	{
		//UE_LOG(LogTemp, Warning, TEXT("wheel %f"), ValueAmount);
		const float ArmLength = CameraArm->TargetArmLength;
		//only allow player zoom out (ValueAmount > 0) when the arm length exceed the arm length min
		if (ArmLength <= ArmMin && ValueAmount > 0.f)
		{
			CameraArm->TargetArmLength = ValueAmount * ZoomSpeed + ArmLength;
		}
		//only allow player to zoom in (ValueAmount < 0) when the arm length exceed the arm length max
		else if (ArmLength >= ArmMax && ValueAmount < 0.f)
		{
			CameraArm->TargetArmLength = ValueAmount * ZoomSpeed + ArmLength;
		}
		//if the arm length is between the min and max, free zoom allowed
		else if (ArmLength > ArmMin && ArmLength < ArmMax)
		{
			//UE_LOG(LogTemp, Warning, TEXT("ArmLength %f"), ValueAmount);
			CameraArm->TargetArmLength = ValueAmount * ZoomSpeed + ArmLength;
		}
		//Change the speed and acceleration and deceleration based on the camera arm length, longer length move faster
		GetCharacterMovement() -> MaxWalkSpeed = (MoveSpeed * CameraArm->TargetArmLength) / 1000;
		//GetCharacterMovement() -> MaxAcceleration = (AccSpeed * CameraArm->TargetArmLength) / 1000;
	}
}

//Allow player to rotate vertically by using the RF keys
void ARTSCharacter::RotatePitch(const FInputActionValue& Value)
{
	const float DirectionValue = Value.Get<float>();
	if (Controller && DirectionValue != 0.f)
	{
		//UE_LOG(LogTemp, Warning, TEXT("pressed rf"));
		AddControllerPitchInput(DirectionValue * 0.5);
	}
}

//Left button pressed
void ARTSCharacter::LeftPressed(const FInputActionValue& Value)
{
	if (Hud)
	{
		//Start HUD process
		Hud -> InitialPoint=Hud->GetMousePosition();
		Hud -> StartSelection=true;
		//Applied selection shader on the actors that we select
		if (!ControlledUnit.IsEmpty())
		{
			for (auto CurrActor : ControlledUnit)
			{
				ChangeCustomDepth(false,CurrActor, 10);
				CurrActor->bIsSelect = false;
				if (CurrActor->IsValidLowLevel() && CurrActor->WidgetInstance)
				{
					CurrActor->SetDeselected();
				}
			}
		}
		//clear the array
		SelectedActor.Empty();
		ControlledUnit.Empty();
		//if we got current selected building from the clickable actor component
		//if we got previous building we deselect all the actors when we pressed left button second time
		if (SelectedBuilding == nullptr || SelectedBuilding != PreviousBuilding)
		{
			if (PreviousBuilding)
			{
				//get all the actor component from that building
				TArray<UActorComponent*> Components;
				PreviousBuilding->GetComponents(UActorComponent::StaticClass(), Components);
				if (!Components.IsEmpty())
				{
					//loop through all the components
					for (auto CurrComponent : Components)
					{
						// try to find the clickable component
						if (UClickableActorComponent* Clickable = Cast<UClickableActorComponent>(CurrComponent))
						{
							//Remove the selected shader
							Clickable->ChangeCustomDepth(false, 0);
							//set it to not clicked by player
							Clickable->IsClicked = false;
							PreviousBuilding->SetDeselection();
							break;
						}
					}
				}
				//Keep selected building variable emptyaaw
				SelectedBuilding = nullptr;
			}
		}
		//update the current select building to previous building and clear the current building
		PreviousBuilding = SelectedBuilding;
		SelectedBuilding = nullptr;
	}
}

//when we release the left button, create the selection shader to all actors
void ARTSCharacter::LeftRelease(const FInputActionValue& Value)
{
	if (Hud)
	{
		Hud -> StartSelection=false;
		//Get all the actor within the HUD(we have a filter in RTS_HUD class to the NPC unit only)
		SelectedActor = Hud->GetActor;
		//loop through it
		for (auto CurrActor : SelectedActor)
		{
			// if it owned by a player controller
			if (CurrActor->ControlledController)
			{
				// and this controller is same as this(first location) controller
				if (CurrActor->ControlledController == this->GetController())
				{
					//apply the blue selection shader
					ChangeCustomDepth(true,CurrActor, 10);
					//Add those units that belongs to us to a new TArray and waiting for our control
					ControlledUnit.Add(CurrActor);
					CurrActor->bIsSelect = true;
				}
			}
		}
		//Only call the Unit UI when we select one of them
		if (ControlledUnit.Num() == 1)
		{
			ControlledUnit[0]->SetSelection();
		}
	}
}

//Call the selection shader to display on the actor
void ARTSCharacter::ChangeCustomDepth(bool ActiveSelectionShader, AActor* CurrentActor, int StencilValue)
{
	if (CurrentActor && CurrentActor->IsValidLowLevel())
	{
		//loop through all the components and add the shader to them
		TArray<UActorComponent*>  MeshComponents;
		CurrentActor->GetComponents(UStaticMeshComponent::StaticClass(), MeshComponents);
		if (MeshComponents.Num() != 0)
		{
			for (auto Component: MeshComponents)
			{
				if (UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(Component))
				{
					StaticMesh->SetRenderCustomDepth(ActiveSelectionShader);
					StaticMesh->SetCustomDepthStencilValue(StencilValue);
					//UE_LOG(LogTemp,Warning,TEXT("Changing"));
				}
				//UE_LOG(LogTemp,Warning,TEXT("no skinned"));
			}
			//UE_LOG(LogTemp,Warning,TEXT("no mesh"));
		}
	}
}

//called when right button pressed, currently it only control the movement of the unit
void ARTSCharacter::RightPressed(const FInputActionValue& Value)
{
	//if we have the units that we can control
	if (!ControlledUnit.IsEmpty())
	{
		//use this variable to store the raytrace data
		FHitResult Hit;
		if (PlayerController)
		{
			//Get the raytrace data under the cursor
			PlayerController->GetHitResultUnderCursor(ECC_GameTraceChannel2,true,Hit);
		}
		//When our cursor in attack mode (we are hovering the enemy building for now, need to expend to enemy units)
		//we need give the order to unit that we want to attack the target
		if (PlayerController->CurrentMouseCursor == EMouseCursor::TextEditBeam)
		{
			//use this variable to save the attack target
 			AActor* AttackTarget = Hit.GetActor();
			//if we have a attack target
			if (AttackTarget)
			{
				//Loop through all the controlled unit that we selected
				for(auto CurrentUnit:ControlledUnit)
				{
					//Make server handle out attack behaviour
					ServerAttackTarget(CurrentUnit, AttackTarget);
				}
			}
		}
		//When the cursor is the change to the yellow, which player is hovering to a resources point
		else if (PlayerController->CurrentMouseCursor == EMouseCursor::ResizeLeftRight)
		{
			//we get the actor under the cursor which should be a resources center
			AActor* ResourcesPointActor = Hit.GetActor();
			//if the target resources point is valid
			if (AResourcesPoint* TargetResourcesPoint = Cast<AResourcesPoint>(ResourcesPointActor))
			{
				//we use this to track if we have worker exist in our selected units
				bool bHasWorkerSelected = false;
				for (auto CurrUnit : ControlledUnit)
				{
					//we check is there any workers that we currently select
					if (AWorkerCharacter* CurrWorker = Cast<AWorkerCharacter>(CurrUnit))
					{
						//we have at least one worker exist in our selected units
						bHasWorkerSelected = true;
						//we check are we order the unit to the same resources point, function will only active when we assign a new target
						if (CurrWorker->TargetResourcesPoint != TargetResourcesPoint)
						{
							ServerGatheringResources(CurrWorker, TargetResourcesPoint);
						}
					}
				}
				if (!bHasWorkerSelected)
				{
					GEngine->AddOnScreenDebugMessage(1,2.f,FColor::Blue,TEXT("There are no worker in your selection"));
				}
			}
		}
		else
		{
			for (int i=0;i<ControlledUnit.Num();i++)
			{
				//the formation calculation, make sure when we select multiple units, each of them are not targeting to the same location
				const FVector NewLocation = Hit.Location+FVector(i/2*300,i%2*300,0);
				if (ANPC* Unit = Cast<ANPC>(ControlledUnit[i]))
				{
					//I make server set the replicated TargetLocation variable, and make the server move the AI for us
					ServerSetLocationVar(NewLocation);
					ServerSetNewLocation(Unit);
					ServerClearTarget(Unit);
					//ServerClearTarget(Unit);
					//Worker->MoveToTargetLocation(NewLocation);
					/*
					ARTSAIController* AIController = Cast<ARTSAIController>(Worker->GetController());
					if (AIController)
					{
						//AIController->MoveToTargetLocation(Location);
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(Worker->GetController(),NewLocation);
						UE_LOG(LogTemp, Warning, TEXT("Has controller and Moving"));
					}
					UE_LOG(LogTemp, Warning, TEXT("Moving"));*/
					/*
					ARTSAIController* AIController = Cast<ARTSAIController>(Worker->GetController());
					if (AIController)
					{
						//AIController->MoveToTargetLocation(Location);
						UAIBlueprintHelperLibrary::SimpleMoveToLocation(Worker->GetController(),NewLocation);
						UE_LOG(LogTemp, Warning, TEXT("Has controller and Moving"));
					}*/
				}
			}
		}
	}
}

void ARTSCharacter::ServerGatheringResources_Implementation(AWorkerCharacter* CurrWorker, AResourcesPoint* ResourcesPoint)
{
	CurrWorker->CollectResources(ResourcesPoint, PlayerSpawnedBuildings);
}

void ARTSCharacter::ServerAttackTarget_Implementation(ANPC* AttackUnit, AActor* Target)
{
	AttackUnit->AttackTarget(Target);
}


void ARTSCharacter::ServerSetLocationVar_Implementation(const FVector Location)
{
	TargetLocation = Location;
}

void ARTSCharacter::ServerSetNewLocation_Implementation(ANPC* Unit)
{
	if (FXCursor)
	{
		MulticastSpawnCursorParticles(FXCursor);
	}
	if (Unit->IsValidLowLevel())
	{
		Unit->MoveToTargetLocation(TargetLocation);
	}
}

//also called when left button clicked
void ARTSCharacter::BindSpawnBuildingWithLeftClick(const FInputActionValue& Value)
{
	//Check the place status and how much the resources left for the current player
	if (PlacementStatus && PlayerState)
	{
		//spawn the building if we have enough resources
		if (PlayerState->MetalCount >= Cast<ABuilding>(PlaceablActor)->MetalCost && PlayerState->WoodCount >= Cast<ABuilding>(PlaceablActor)->WoodCost)
		{
			SpawnBuilding();
		}
		//otherwise, display the massanges
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Not Enough Resources"));
		}
	}
}






void ARTSCharacter::ServerSetBuildingLocationVar_Implementation(const FVector Location)
{
	BuildingLocation = Location;
}

void ARTSCharacter::ServerSetBuildingLocation_Implementation(ABuilding* Building)
{
	if (Building)
	{
		Building->SetLocation(BuildingLocation);
	}
}

void ARTSCharacter::ClientSetBuildingLocation_Implementation(ABuilding* Building)
{
	
}

//Update the place location in tick
void ARTSCharacter::PlaceLocation()
{
	FVector WorldLocation;
	FVector WorldDirection;
	if (PlayerController)
	{
		PlayerController->DeprojectMousePositionToWorld(WorldLocation,WorldDirection);
		//UE_LOG(LogTemp,Warning,TEXT("Has controller"))
	}
	FHitResult HitResult;
	//Create the line trace, the channel GameTraceChannel2 is the custom channel that I created in Unreal editor which represent all the lands
	GetWorld()->LineTraceSingleByChannel( HitResult,WorldLocation,WorldDirection*10000+WorldLocation , ECC_GameTraceChannel1);
	if (HitResult.bBlockingHit && PlaceablActor)
	{
		ServerSetBuildingLocationVar(HitResult.Location);
		ABuilding* Building = Cast<ABuilding>(PlaceablActor);
		if (Building)
		{
			ServerSetBuildingLocation(Building);
			//UE_LOG(LogTemp,Warning,TEXT("Server Setlocation"))
		}
	}
}

//use this function to spawn the building from server
void ARTSCharacter::ServerSpawnBuilding_Implementation(TSubclassOf<ABuilding> NewBuilding, FVector NewBuildingLocation, FRotator NewBuildingRotation)
{
	if (HasAuthority())
	{
		//we spawn the building and save a reference of it
		ABuilding* Building = Cast<ABuilding>(GetWorld()->SpawnActor(NewBuilding,&NewBuildingLocation, &NewBuildingRotation));
		//add to the spawned buildings array, this can make we search buildings easier in the future
		PlayerSpawnedBuildings.Add(Building);
		//if we spawn it successfully 
		if (Building)
		{
			//we set the owner of that to this player, and make sure server handle it
			ServerSetOwningPlayer(Building);
			//Once we finished player assign, then we can disable to Placing status of that building which means it can be attack by any other enemy units
			Building->bIsPlacing = false;
			//Then we clear the foliage around it, and multicast it to all connected client
			MultiCastClearFoliage(Building);
		}
	}
}

//we this to multicast the clear foliage effect to all the connected clients
void ARTSCharacter::MultiCastClearFoliage_Implementation(AActor* Actor)
{
	if (Actor)
	{
		//Use this to save the hit results
		TArray<FHitResult> HitResults;
		//this is the collision sphere that we will use to do the sweep
		FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(1100.0f);
		//draw the debug sphere
		DrawDebugSphere(GetWorld(), Actor->GetActorLocation(), CollisionSphere.GetSphereRadius(), 50, FColor::Red, false, 1.0f, 0,0);
		//we start the sweep and check if it is work
		if(GetWorld()->SweepMultiByChannel(HitResults, Actor->GetActorLocation(),Actor->GetActorLocation(), FQuat::Identity, ECC_WorldStatic, CollisionSphere)){
			//loop through the results
			for(auto& Hit : HitResults){
				//check if the hit component is the UFoliageInstancedStaticMeshComponent or not
				if (UFoliageInstancedStaticMeshComponent* FoliageMeshComp = Cast<UFoliageInstancedStaticMeshComponent>(Hit.Component))
				{
					//if it is, we remove it
					FoliageMeshComp->RemoveInstance(Hit.Item);
				}
			}	
		}
	}
}


void ARTSCharacter::SpawnBuilding()
{
	UplacableActorComponent* ActorComponent = nullptr;
	if (PlaceablActor)
	{
		ActorComponent = Cast<UplacableActorComponent>(PlaceablActor->GetComponentByClass(UplacableActorComponent::StaticClass()));
	}
	if (ActorComponent)
	{
		if (PlacementStatus && ActorComponent->CanPlop)
		{
			// set the spawn location for worker
			if (!SelectedActor.IsEmpty())
			{
				for (int i=0;i<SelectedActor.Num();i++)
				{
					if (Cast<AWorkerCharacter>(SelectedActor[i]))
					{
						SelectedActor[i]->Goal=PlaceablActor->GetActorLocation();
						SelectedActor[i]->Destination();
					}
				}
			}
			ServerSpawnBuilding(PlaceableActorClass, PlaceablActor->GetActorLocation(), PlaceablActor->GetActorRotation());
			//UE_LOG(LogTemp,Warning, TEXT("Spawn building"));
			if (PlaceablActor)
			{
				UE_LOG(LogTemp,Warning,TEXT("Trying to Destroy placement mode building"))
				ServerDestroyActor(PlaceablActor);
			}
			ServerSetPlacementStatus(false);
			ServerCalculateCost(Cast<ABuilding>(PlaceablActor)->MetalCost * -1,Cast<ABuilding>(PlaceablActor)->WoodCost * -1);
		}
	}
}

// Allow player get into the placement mode and placing the building
void ARTSCharacter::ServerSetPlaceEnable_Implementation(bool IsEnabled, TSubclassOf<ABuilding> Building)
{
	if (IsEnabled != PlacementStatus)
	{
		PlaceableActorClass = Building;
		ServerSetPlacementStatus(IsEnabled);
		if (PlacementStatus)
		{
			const FVector SpawnLocation = this->GetActorLocation();
			ServerSpawnPlaceableBuilding(SpawnLocation);
			UActorComponent* Component = PlaceablActor->GetComponentByClass(UClickableActorComponent::StaticClass());
			if(UClickableActorComponent* ClickableActorComponent = Cast<UClickableActorComponent>(Component))
			{
				ClickableActorComponent->DestroyComponent();
			}
			ServerAddPlaceableComp();
			UE_LOG(LogTemp, Warning, TEXT("PloppableAC assigned"))
		}
		else
		{
			if (PlaceablActor)
			{
				PlaceablActor->Destroy();
			}
		}
	}
}


void ARTSCharacter::ServerSpawnPlaceableBuilding_Implementation(FVector Location)
{
	if (HasAuthority())
	{
		PlaceablActor = GetWorld()->SpawnActor(PlaceableActorClass, &Location);
		if (ABuilding* Building = Cast<ABuilding>(PlaceablActor))
		{
			ServerSetOwningPlayer(Building);
			Building->ServerSetPlaceableBuildingCollision();
		}
	}
}

void ARTSCharacter::SetPlaceEnable(bool IsEnabled, TSubclassOf<ABuilding> Building)
{
	ServerSetPlaceEnable(IsEnabled, Building);
}

void ARTSCharacter::ServerCalculateCost_Implementation(float MetalNum, float WoodNum)
{
	if (HasAuthority())
	{
		if (PlayerState)
		{
			PlayerState->MetalCount += MetalNum;
			PlayerState->WoodCount += WoodNum;
		}
	}
}


// Called every frame
void ARTSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (ViewportSize != FVector2d(0,0))
	{
		float x,y;
		GetWorld()->GetFirstPlayerController()->GetMousePosition(x,y);
		//UE_LOG(LogTemp, Warning, TEXT("x = %f y = %f"), x,y);
		if (x <= 30)
		{
			if (Controller)
			{
				FVector Right = this->GetActorRightVector();
				AddMovementInput(Right, -1);
			}
		}
		else if (x >= ViewportSize.X - 30)
		{
			if (Controller)
			{
				FVector Right = this->GetActorRightVector();
				AddMovementInput(Right, 1);
			}
		}
		else if (y <= 30)
		{
			if (Controller)
			{
				const FVector Forward = this->GetActorForwardVector();
				AddMovementInput(FVector(Forward.X,Forward.Y,0), 1);
			}
		}
		else if (y >= ViewportSize.Y - 30)
		{
			if (Controller)
			{
				const FVector Forward = this->GetActorForwardVector();
				AddMovementInput(FVector(Forward.X,Forward.Y,0), -1);
			}
		}
	}*/

	if (PlacementStatus)
	{
		PlaceLocation();
	}

	if (PlayerState == nullptr)
	{
		PlayerState = Cast<AMulti_PlayerState>(this->GetPlayerState());
	}
	if (PlaceablActor)
	{
		//UE_LOG(LogTemp,Warning,TEXT("Has placeable building"));
		//UE_LOG(LogTemp,Warning,TEXT("Team1 %d"), GameState->TeamOne.Num());
		//UE_LOG(LogTemp,Warning,TEXT("Team2 %d"), GameState->TeamTwo.Num());
	}
}

// Called to bind functionality to input
void ARTSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Bind Input Action with the function
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARTSCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARTSCharacter::Look);
		EnhancedInputComponent->BindAction(MoveADAction, ETriggerEvent::Triggered, this, &ARTSCharacter::MoveAD);
		EnhancedInputComponent->BindAction(MoveUpAction, ETriggerEvent::Triggered, this, &ARTSCharacter::MoveUp);
		EnhancedInputComponent->BindAction(Rotation, ETriggerEvent::Triggered, this, &ARTSCharacter::Rotate);
		EnhancedInputComponent->BindAction(HoldRight, ETriggerEvent::Triggered, this, &ARTSCharacter::HoldingRight);
		EnhancedInputComponent->BindAction(HoldRight, ETriggerEvent::Completed, this, &ARTSCharacter::ReleaseRight);
		EnhancedInputComponent->BindAction(Zoom, ETriggerEvent::Triggered, this, &ARTSCharacter::CameraZoom);
		EnhancedInputComponent->BindAction(RotatePitchAction, ETriggerEvent::Triggered, this, &ARTSCharacter::RotatePitch);
		EnhancedInputComponent->BindAction(LeftClick, ETriggerEvent::Started, this, &ARTSCharacter::LeftPressed);
		EnhancedInputComponent->BindAction(LeftClick, ETriggerEvent::Started, this, &ARTSCharacter::BindSpawnBuildingWithLeftClick);
		EnhancedInputComponent->BindAction(LeftClick, ETriggerEvent::Completed, this, &ARTSCharacter::LeftRelease);
		EnhancedInputComponent->BindAction(RightClick, ETriggerEvent::Completed, this, &ARTSCharacter::RightPressed);
	}

}

void ARTSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARTSCharacter, TargetLocation);
	DOREPLIFETIME(ARTSCharacter, BuildingLocation);
	DOREPLIFETIME(ARTSCharacter, PlacementStatus);
	DOREPLIFETIME(ARTSCharacter, PlaceablActor);
	DOREPLIFETIME(ARTSCharacter, PlaceableActorClass);
	DOREPLIFETIME(ARTSCharacter, PlayerSpawnedBuildings);
}


void ARTSCharacter::ServerClearTarget_Implementation(ANPC* Unit)
{
	Unit->TargetEnemyRef = nullptr;
	if (AWorkerCharacter* Worker = Cast<AWorkerCharacter>(Unit))
	{
		Worker->TargetResourcesPoint = nullptr;
	}
}

void ARTSCharacter::ServerCreateUI_Implementation()
{
	if (HasAuthority())
	{
		ClientCreateUI();
	}
}

void ARTSCharacter::ClientCreateUI_Implementation()
{
	if (MainUI)
	{
		if (UUserWidget* WidgetInstance=CreateWidget<UUserWidget>(GetWorld(), MainUI))
		{
			WidgetInstance->AddToViewport();
		}
	}
}


void ARTSCharacter::ServerAddPlaceableComp_Implementation()
{
	if (PlaceablActor)
	{
		UActorComponent* Placable = PlaceablActor->AddComponentByClass(UplacableActorComponent::StaticClass(), true, PlaceablActor->GetTransform(), false);
	
	
		if (Placable)
		{
			Placable->SetIsReplicated(true);
		}
	}
}


void ARTSCharacter::MulticastSpawnCursorParticles_Implementation(UNiagaraSystem* NiagaraSystem)
{
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, TargetLocation, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
}


void ARTSCharacter::ServerDestroyActor_Implementation(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("Destroyed successfully"))
	Actor->Destroy();
}

void ARTSCharacter::ServerSetOwningPlayer_Implementation(ABuilding* Building)
{
	if (PlayerState)
	{
		Building->SetOwningController(this->GetController());
	}
}

void ARTSCharacter::ServerSetPlacementStatus_Implementation(bool bSetPlaceMode)
{
	PlacementStatus = bSetPlaceMode;
}

