// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankBase.h"
#include "Tank.generated.h"

class UInputAction;
class UParticleSystemComponent;
class UInputMappingContext;
struct FInputActionInstance;

UCLASS()
class TOONTANKS_API ATank : public ATankBase
{
	GENERATED_BODY()

public:
	ATank();

	// Input
	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMapping;
	
	UPROPERTY(EditAnywhere)
	UInputAction* MoveForwardAction;

	UPROPERTY(EditAnywhere)
	UInputAction* MoveRightAction;

	UPROPERTY(EditAnywhere)
	UInputAction* RotateTurretAction;

	UPROPERTY(EditAnywhere)
	UInputAction* FireAction;
	
	UPROPERTY(EditAnywhere)
	float MoveSpeed = 100.f;

	UPROPERTY(EditAnywhere)
	float RotateSpeed = 100.f;

	UPROPERTY(EditAnywhere)
	float GroundOffset = 100.f;

	UPROPERTY(EditAnywhere)
	float GravityMultiplier = 1.f;
	
	UPROPERTY(EditAnywhere)
	TArray<FComponentReference> DustParticles;

	UPROPERTY()
	TArray<UParticleSystemComponent*> DustParticleSystems;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetGamepadInputActive(bool IsActive);

	virtual void HandleDestruction() override;

	virtual void Fire() override;

	class APlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UCameraShakeBase> HitCameraShake;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> DestroyCameraShake;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	FVector MoveDirection;
	FRotator LastTurretRotation;
	bool IsMoving = false;
	bool IsGamepadInput;

	UFUNCTION()
	void Move(const FInputActionInstance& Instance);

	UFUNCTION()
	void Rotate(const FInputActionInstance& Instance);

	UFUNCTION()
	void RotateTurretInDirection(FVector Direction);

	class FCTweenInstance* MoveAnimTween;
	void OnStartMoving();
	void OnStopMoving();
	void CheckGround();
	void UpdateDustParticles(bool Activate) const;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
