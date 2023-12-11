// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker_Widget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWorker_Widget::NativeConstruct()
{
	Super::NativeConstruct();
	
	

	// get the button reference
	UButton* ResourceCenter = Cast<UButton>(GetWidgetFromName(TEXT("Resource")));
	UButton* Arsenal = Cast<UButton>(GetWidgetFromName(TEXT("Arsenal")));

	// bind the event to the button
	if (ResourceCenter)
	{
		ResourceCenter->OnClicked.AddDynamic(this, &UWorker_Widget::PlaceResourceCenter);
	}

	if (Arsenal)
	{
		Arsenal->OnClicked.AddDynamic(this, &UWorker_Widget::PlaceArsenal);
	}
	
	Player = Cast<ARTSCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
}

void UWorker_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UWorker_Widget::PlaceResourceCenter()
{
	if (Player)
	{
		if (ResourceCenter_Build)
		{
			Player->SetPlaceEnable(true, ResourceCenter_Build);
			UE_LOG(LogTemp,Warning,TEXT("Button work"))
		}
	}
}

void UWorker_Widget::PlaceArsenal()
{
	if (Player)
	{
		if (Arsenal_Build)
		{
			Player->SetPlaceEnable(true, Arsenal_Build);
			UE_LOG(LogTemp,Warning,TEXT("Button work"))
		}
		
	}
}
