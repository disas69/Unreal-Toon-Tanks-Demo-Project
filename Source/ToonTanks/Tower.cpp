// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerPawn == nullptr)
	{
		PlayerPawn = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (IsPlayerInRange())
	{
		FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
		Direction.Z = 0.f;
		Direction.Normalize();

		RotateTurret(Direction);
	}
}

void ATower::HandleDestruction()
{
	Super::HandleDestruction();
	Destroy();
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));

	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATower::CheckFireCondition, FireRate, true);
}

void ATower::CheckFireCondition()
{
	if (IsPlayerInRange())
	{
		Fire();
	}
}

bool ATower::IsPlayerInRange()
{
	if (PlayerPawn && !PlayerPawn->IsHidden())
	{
		float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
		if (DistanceToPlayer <= FireRange)
		{
			return true;
		}
	}

	return false;
}
