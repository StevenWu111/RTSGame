// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUserWidget.h"

#include "Multi_PlayerState.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Net/UnrealNetwork.h"

void UMainUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Player = Cast<ARTSCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UMainUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (Player && Player->PlayerState)
	{
		MetalCount->SetText(FText::AsNumber(Player->PlayerState->MetalCount));
		WoodCount->SetText(FText::AsNumber(Player->PlayerState->WoodCount));
	}
}