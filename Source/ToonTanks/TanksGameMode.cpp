// Fill out your copyright notice in the Description page of Project Settings.


#include "TanksGameMode.h"

#include "Tank.h"
#include "TankPlayerController.h"
#include "Tower.h"
#include "Kismet/GameplayStatics.h"

void ATanksGameMode::BeginPlay()
{
	Super::BeginPlay();

	PlayerTank = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	PlayerController = Cast<ATankPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}

void ATanksGameMode::ActorDied(AActor* DeadActor)
{
	if (DeadActor == PlayerTank)
	{
		PlayerTank->HandleDestruction();

		if (PlayerController)
		{
			PlayerController->EnablePlayerInput(false);
		}
	}
	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))
	{
		DestroyedTower->HandleDestruction();
	}
}
