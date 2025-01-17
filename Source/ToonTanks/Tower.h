// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "Tower.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATower : public ATankBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	float FireRange = 500.f;

	UPROPERTY(EditAnywhere)
	float FireRate = 2.f;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HandleDestruction() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	class ATank* PlayerPawn;
	FTimerHandle FireRateTimerHandle;

	void CheckFireCondition();
	bool IsPlayerInRange();
};
