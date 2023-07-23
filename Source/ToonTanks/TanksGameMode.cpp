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

	StartGame();
	HandleGameStart();

	TargetTurretsCount = GetTargetTurretsCount();
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
		
		GameOver(false);
	}
	else if (ATower* DestroyedTower = Cast<ATower>(DeadActor))
	{
		DestroyedTower->HandleDestruction();

		TargetTurretsCount--;
		if (TargetTurretsCount == 0)
		{
			GameOver(true);
		}
	}
}

void ATanksGameMode::HandleGameStart()
{
	if (PlayerController)
	{
		if (StartDelay > 0.f)
		{
			PlayerController->EnablePlayerInput(false);

			FTimerHandle PlayerEnableHandle;
			FTimerDelegate PlayerEnableDelegate = FTimerDelegate::CreateUObject(PlayerController, &ATankPlayerController::EnablePlayerInput, true);

			GetWorldTimerManager().SetTimer(PlayerEnableHandle, PlayerEnableDelegate, StartDelay, false);
		}
		else
		{
			PlayerController->EnablePlayerInput(true);
		}
	}
}

int32 ATanksGameMode::GetTargetTurretsCount()
{
	TArray<AActor*> Turrets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATower::StaticClass(), Turrets);

	return Turrets.Num(); 
}
