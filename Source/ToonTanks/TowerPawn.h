// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "TowerPawn.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATowerPawn : public ATankBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float FireRange = 500.f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class ATankPawn* PlayerPawn;

	void RotateTurret(FVector LookAtTarget);
};
