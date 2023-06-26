// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ATankPawn::ATankPawn()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (PlayerController && !IsGamepadInput)
	{
		FHitResult CursorHitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHitResult))
		{
			DrawDebugSphere(GetWorld(), CursorHitResult.ImpactPoint, 25.f, 8, FColor::Red, false, 0.1f);

			FVector HitLocation = CursorHitResult.ImpactPoint;
			FVector Direction = HitLocation - GetActorLocation();
			Direction.Z = 0.f;
			Direction.Normalize();

			RotateTurret(Direction);
		}
	}
}

void ATankPawn::SetGamepadInputActive(bool IsActive)
{
	IsGamepadInput = IsActive;
}

void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetController());
}

void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATankPawn::Move);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATankPawn::Rotate);
	PlayerInputComponent->BindVectorAxis(TEXT("RotateTurret"), this, &ATankPawn::RotateTurretInDirection);
	
}

void ATankPawn::Move(float Value)
{
	MoveDirection = Value;
	FVector Forward = GetActorForwardVector();
	AddActorWorldOffset(Forward * Value * MoveSpeed * GetWorld()->DeltaTimeSeconds, true);
}

void ATankPawn::Rotate(float Value)
{
	if (MoveDirection < 0.f)
	{
		Value *= -1;
	}
	
	FRotator Rotation = FRotator(0.f, Value * RotateSpeed * GetWorld()->DeltaTimeSeconds, 0.f);
	AddActorWorldRotation(Rotation, true);
}

void ATankPawn::RotateTurret(FVector LookAtTarget)
{
	FRotator TargetRotation = FRotator(0.f, LookAtTarget.Rotation().Yaw, 0.f);
	FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
	TurretMesh->SetWorldRotation(Rotation);
}

void ATankPawn::RotateTurretInDirection(FVector Direction)
{
	UE_LOG(LogTemp, Warning, TEXT("Direction: %s"), *Direction.ToString());
	FRotator TargetRotation = FRotator(0.f, Direction.Rotation().Yaw, 0.f);
	FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
	TurretMesh->SetWorldRotation(Rotation);
}


