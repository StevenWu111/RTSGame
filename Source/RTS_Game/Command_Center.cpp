// Fill out your copyright notice in the Description page of Project Settings.


#include "Command_Center.h"

ACommand_Center::ACommand_Center()
{
	WoodCost=Wood;
	MetalCost=Metal;
	bReplicates = true;
	BuildingMesh->SetIsReplicated(true);
	NetPriority = 3.0f;
}
