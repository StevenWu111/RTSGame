// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceCenter_Widget.h"

#include "Components/Button.h"

void UResourceCenter_Widget::ButtonClicked()
{
	Super::ButtonClicked();
	StartGenerateWorker=true;
}

void UResourceCenter_Widget::NativeConstruct()
{
	Super::NativeConstruct();

	UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Worker")));

	// bind the event to the button
	if (Button)
	{
		Button->OnClicked.AddDynamic(this, &UBuilding_Widget::ButtonClicked);
	}
}
