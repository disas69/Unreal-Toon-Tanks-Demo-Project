// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "TankPawn.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATankPawn : public ATankBase
{
	GENERATED_BODY()

public:
	ATankPawn();

	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere)
	float RotateSpeed = 100.f;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	class APlayerController* PlayerController;

	float MoveDirection;

	void Move(float Value);
	void Rotate(float Value);
	void RotateTurret(FVector LookAtTarget);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
