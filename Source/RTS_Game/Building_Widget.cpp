// Fill out your copyright notice in the Description page of Project Settings.


#include "Building_Widget.h"

#include "Components/Button.h"

/*void UBuilding_Widget::NativeConstruct()
{
	/*Super::NativeConstruct();

	// get the button reference
	UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Solider")));

	// bind the event to the button
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UBuilding_Widget::ButtonClicked);
	}#1#
}*/

void UBuilding_Widget::ButtonClicked()
{
	/*UE_LOG(LogTemp, Warning, TEXT("Button Clicked!"));

	StartGenerate=true;*/
	
}

void UBuilding_Widget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{

	Super::NativeTick(MyGeometry, InDeltaTime);
	if(Building)
	{
		HealthBar->SetPercent(Building->BuildingHP/100);
		FNumberFormattingOptions Opts;
		Opts.SetMaximumFractionalDigits(0);
		CurrentHealthLabel->SetText(FText::AsNumber(Building->BuildingHP, &Opts));
	}
}
