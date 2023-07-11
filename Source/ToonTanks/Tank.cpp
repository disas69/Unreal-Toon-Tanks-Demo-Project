// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATank::ATank()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ATank::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerController && !IsGamepadInput)
	{
		FHitResult CursorHitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHitResult))
		{
			DrawDebugSphere(GetWorld(), CursorHitResult.ImpactPoint, 25.f, 8, FColor::Red, false, 0.1f);

			FVector HitLocation = CursorHitResult.ImpactPoint;
			HitLocation.Z = GetActorLocation().Z;
			FVector Direction = HitLocation - GetActorLocation();
			Direction.Normalize();

			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Direction * 1000.f, FColor::Red, false, 0.1f, 0, 5.f);

			RotateTurret(Direction);
		}
	}
}

void ATank::SetGamepadInputActive(bool IsActive)
{
	IsGamepadInput = IsActive;
}

void ATank::HandleDestruction()
{
	Super::HandleDestruction();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
}

void ATank::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetController());
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATank::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATank::Rotate);
	PlayerInputComponent->BindVectorAxis(TEXT("RotateTurret"), this, &ATank::RotateTurretInDirection);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
	
}

void ATank::Move(float Value)
{
	MoveDirection = Value;
	FVector Forward = GetActorForwardVector();
	AddActorWorldOffset(Forward * Value * MoveSpeed * GetWorld()->DeltaTimeSeconds, true);
}

void ATank::Rotate(float Value)
{
	if (MoveDirection < 0.f)
	{
		Value *= -1;
	}
	
	FRotator Rotation = FRotator(0.f, Value * RotateSpeed * GetWorld()->DeltaTimeSeconds, 0.f);
	AddActorWorldRotation(Rotation, true);
}

void ATank::RotateTurretInDirection(FVector Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
	FRotator TargetRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
	FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
	TurretMesh->SetWorldRotation(Rotation);
}


