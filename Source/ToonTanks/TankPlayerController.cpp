// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"

void ATankPlayerController::EnablePlayerInput(bool bEnable)
{
	if (bEnable)
	{
		GetPawn()->EnableInput(this);
	}
	else
	{
		GetPawn()->DisableInput(this);
	}

	bShowMouseCursor = bEnable;
}
