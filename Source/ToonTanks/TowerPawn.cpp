// Fill out your copyright notice in the Description page of Project Settings.


#include "TowerPawn.h"
#include "TankPawn.h"
#include "Kismet/GameplayStatics.h"

void ATowerPawn::Tick(float DeltaTime)
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

void ATowerPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = Cast<ATankPawn>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATowerPawn::RotateTurret(FVector LookAtTarget)
{
	FRotator TargetRotation = FRotator(0.f, LookAtTarget.Rotation().Yaw, 0.f);
	FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
	TurretMesh->SetWorldRotation(Rotation);
}
