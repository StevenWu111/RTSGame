// Fill out your copyright notice in the Description page of Project Settings.


#include "Arsenal_Widget.h"

#include "Components/Button.h"

void UArsenal_Widget::ButtonClicked()
{
	Super::ButtonClicked();

	UE_LOG(LogTemp, Warning, TEXT("Button Clicked!"));

	StartGenerateSolider=true;
	
}

void UArsenal_Widget::NativeConstruct()
{
	Super::NativeConstruct();
	// get the button reference
	UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Solider")));

	// bind the event to the button
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UBuilding_Widget::ButtonClicked);
	}
}
