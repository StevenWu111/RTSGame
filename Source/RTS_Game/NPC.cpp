// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC.h"

#include "RTSAIController.h"
#include "RTS_PlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ANPC::ANPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	MeshComponent->SetupAttachment(GetRootComponent());
	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Range"));
	AttackRange->SetupAttachment(RootComponent);
	bReplicates = true;
	MeshComponent->SetIsReplicated(true);

	NetCullDistanceSquared = 224999997440.0;

	
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(EMovementMode::MOVE_NavWalking);
	}
}

// Called when the game starts or when spawned
void ANPC::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetEnemyRef && TargetEnemyRef->IsValidLowLevel())
	{
		RotateTowardEnemy(DeltaTime);
	}
	SetActorRotation(GetActorForwardVector().Rotation());

	
	if (UnitHP <= 0 && ControlledController)
	{
		//this->Destroy();
	}
}

// Called to bind functionality to input
void ANPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ANPC::SetSelection()
{
}

void ANPC::SetDeselected()
{
}
void ANPC::Destination()
{
}

void ANPC::MakeServerCalculateDamage()
{
	if (ARTS_PlayerController* PlayerController = Cast<ARTS_PlayerController>(ControlledController))
	{
		MoveToTargetLocation(this->GetActorLocation());
		switch (AgentType)
		{
		case Melee:
			if (ABuilding* BuildingBeingDamaged = Cast<ABuilding>(TargetEnemyRef))
			{
				PlayerController->ServerDealDamageToBuilding(this, BuildingBeingDamaged);
			}
			else if (ANPC* UnitBringDamaged = Cast<ANPC>(TargetEnemyRef))
			{
				//UE_LOG(LogTemp,Warning, TEXT("deal damage"));
				PlayerController->ServerDealDamageToUnit(this, UnitBringDamaged);
			}
			break;
		case Ranger:
			if (ANPC* UnitBringDamaged = Cast<ANPC>(TargetEnemyRef))
			{
				//UE_LOG(LogTemp,Warning, TEXT("deal damage"));
				PlayerController->ServerDealDamageToUnit(this, UnitBringDamaged);
			}
		}
		
	}
}

void ANPC::ClearTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

AController* ANPC::GetCurrentLocalController()
{
	return GetWorld()->GetFirstPlayerController();
}

void ANPC::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANPC, ControlledController);
	DOREPLIFETIME(ANPC, TargetEnemyRef);
	DOREPLIFETIME(ANPC, UnitHP);
}

void ANPC::MoveToTargetLocation(const FVector Location)
{
	ARTSAIController* AIController = Cast<ARTSAIController>(GetController());
	if (AIController)
	{
		//AIController->MoveToTargetLocation(Location);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this->GetController(),Location);
		//UE_LOG(LogTemp, Warning, TEXT("Has controller and Moving"));
	}
}

void ANPC::AttackTarget(AActor* TargetEnemy)
{
	TargetEnemyRef = TargetEnemy;
	switch (AgentType)
	{
	case Melee:
		if(Cast<ABuilding>(TargetEnemyRef))
		{
			if (AttackBuildingList.Contains(TargetEnemyRef))
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ANPC::MakeServerCalculateDamage, AttackFrequency, true, 0.0f);
			}
			else
			{
				MoveToTargetLocation(TargetEnemyRef->GetActorLocation());
			}
		}
		else if(Cast<ANPC>(TargetEnemyRef))
		{
			if (AttackUnitList.Contains(TargetEnemyRef))
			{
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ANPC::MakeServerCalculateDamage, AttackFrequency, true, 0.0f);
			}
			else
			{
				//UE_LOG(LogTemp,Warning,TEXT("move to target"));
				MoveToTargetLocation(TargetEnemyRef->GetActorLocation());
			}
		}
		break;
	case Ranger:
		MoveToTargetLocation(this->GetActorLocation());
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ANPC::MakeServerCalculateDamage, AttackFrequency, true, 0.3);
		break;
	default:
		break;
	}
	
	CurrentStatus = AttackingTargetEnemy;
}

void ANPC::RotateTowardEnemy(float DeltaTime)
{
	if (TargetEnemyRef)
	{
		//const FVector Target = TargetEnemyRef->GetActorLocation();
		//const FRotator TargetRotation = Direction.Rotation();
		const FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), TargetEnemyRef->GetActorLocation());
		FRotator NewRotation = this->GetActorRotation();
		if (abs(TargetRotation.Yaw - NewRotation.Yaw) > 0.05 )
		{
			NewRotation.Yaw = UKismetMathLibrary::RInterpTo(this->GetActorRotation(), TargetRotation, DeltaTime, RotationSpeed).Yaw;
			this->SetActorRotation(NewRotation);
		}
	}
}



