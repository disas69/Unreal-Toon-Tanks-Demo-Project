// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"

#include "FCTween.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	if (PlayerController && InputEnabled() && !IsGamepadInput)
	{
		PlayerController->bShowMouseCursor = true;
		
		FHitResult CursorHitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHitResult))
		{
			// DrawDebugSphere(GetWorld(), CursorHitResult.ImpactPoint, 25.f, 8, FColor::Red, false, 0.1f);

			FVector HitLocation = CursorHitResult.ImpactPoint;
			HitLocation.Z = GetActorLocation().Z;
			FVector Direction = HitLocation - GetActorLocation();
			Direction.Normalize();

			// DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + Direction * 1000.f, FColor::Red, false, 0.1f, 0, 5.f);

			RotateTurret(Direction);
		}
	}
	else
	{
		PlayerController->bShowMouseCursor = false;
	}

	float Scale = FMath::Clamp(MoveDirection.Size(), 0.f, 1.f);
	
	if (Scale > 0.f)
	{
		if (!IsMoving)
		{
			IsMoving = true;
			OnStartMoving();
		}
		
		MoveDirection.Normalize();
		
		AddActorWorldOffset(MoveDirection * Scale * MoveSpeed * DeltaTime, true);

		FRotator TurretRotation = TurretMesh->GetComponentRotation();
		
		// Rotate tank in direction of movement
		FRotator Rotation = UKismetMathLibrary::MakeRotFromX(MoveDirection);
		Rotation.Pitch = 0.f;
		Rotation.Roll = 0.f;
		
		FRotator SmoothRotation = FMath::RInterpTo(GetActorRotation(), Rotation, DeltaTime, 7.5f);
		SetActorRotation(SmoothRotation);

		TurretMesh->SetWorldRotation(TurretRotation);
	}
	else
	{
		if (IsMoving)
		{
			IsMoving = false;
			OnStopMoving();
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

	if (DestroyCameraShake)
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(DestroyCameraShake);
	}
}

void ATank::Fire()
{
	Super::Fire();

	if (HitCameraShake)
	{
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->StartCameraShake(HitCameraShake);
	}
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
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ATank::Rotate);
	PlayerInputComponent->BindVectorAxis(EKeys::Gamepad_Right2D, this, &ATank::RotateTurretInDirection);
	PlayerInputComponent->BindAction(TEXT("Fire"), IE_Pressed, this, &ATank::Fire);
	
}

void ATank::Move(float Value)
{
	MoveDirection.X = Value;
}

void ATank::Rotate(float Value)
{
	MoveDirection.Y = Value;
}

void ATank::RotateTurretInDirection(FVector Direction)
{
	if (IsGamepadInput && Direction.SizeSquared() > 0.1f)
	{
		Direction.Set(-Direction.Y, Direction.X, 0.f);
		// Find LookAt Rotation
		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + Direction);
		
		// Multiply by camera forward vector to get the correct rotation
		FRotator CameraRotation = Camera->GetComponentRotation();
		CameraRotation.Pitch = 0.f;
		CameraRotation.Roll = 0.f;
		TargetRotation = UKismetMathLibrary::ComposeRotators(TargetRotation, CameraRotation);

		FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
		TurretMesh->SetWorldRotation(Rotation);
	}
}

void ATank::OnStartMoving()
{
	if (MoveAnimTween)
	{
		MoveAnimTween->Destroy();
	}
	
	MoveAnimTween = FCTween::Play(0.f, 5.f, [this](float Value)
	{
		FRotator TurretRotation = TurretMesh->GetComponentRotation();
		
		FRotator BaseRotation = BaseMesh->GetRelativeRotation();
		BaseRotation.Pitch = Value;
		BaseMesh->SetRelativeRotation(BaseRotation);

		TurretMesh->SetWorldRotation(TurretRotation);
	}, 0.15f, EFCEase::InOutSine)
	->SetYoyo(true)
	->SetLoops(-1);
}

void ATank::OnStopMoving()
{
	if (MoveAnimTween)
	{
		MoveAnimTween->Destroy();
	}
	
	MoveAnimTween = FCTween::Play(10.f, 0.f, [this](float Value)
	{
		FRotator BaseRotation = BaseMesh->GetRelativeRotation();
		BaseRotation.Pitch = Value;
		BaseMesh->SetRelativeRotation(BaseRotation);
	}, 0.35f, EFCEase::OutBounce);
}


