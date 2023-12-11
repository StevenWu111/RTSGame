// Fill out your copyright notice in the Description page of Project Settings.


#include "RTS_Hud.h"

#include "Arsenal.h"

FVector2d ARTS_Hud::GetMousePosition()
{

	float X;
	float Y;
	GetOwningPlayerController()->GetMousePosition(X,Y);
	return FVector2d(X,Y);
}

void ARTS_Hud::DrawHUD()
{
	Super::DrawHUD();

	if (StartSelection)
	{
		GetActor.Empty();

		
		CurrentPoint=GetMousePosition();
		//Draws a colored untextured quad on the HUD.
		DrawRect(FLinearColor(0,255,255,0.3),InitialPoint.X,InitialPoint.Y,CurrentPoint.X-InitialPoint.X,CurrentPoint.Y-InitialPoint.Y);

		GetActorsInSelectionRectangle<ANPC>(InitialPoint,CurrentPoint,GetActor,true,true);
	}

	
}

void ARTS_Hud::Select()
{
	if (StartSelection)
	{
		
		if (GetActor.Num()>0)
		{
			for (int i=0;i<GetActor.Num();i++)
			{
				GetActor[i]->SetSelection();
			}
		}
	}
}

void ARTS_Hud::Cancel()
{

	if (StartSelection)
	{
		if (GetActor.Num()>0)
		{
			for (int i=0;i<GetActor.Num();i++)
			{
				GetActor[i]->SetDeselected();
			}
		}
	}
}
