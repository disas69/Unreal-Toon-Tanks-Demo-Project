// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "Tank.h"
#include "Kismet/GameplayStatics.h"

void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerPawn)
	{
		float DistanceToPlayer = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());
		if (DistanceToPlayer <= FireRange)
		{
			FVector Direction = PlayerPawn->GetActorLocation() - GetActorLocation();
			Direction.Z = 0.f;
			Direction.Normalize();

			RotateTurret(Direction);
		}
	}

}

void ATower::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ATank>(UGameplayStatics::GetPlayerPawn(this, 0));
}
