// Fill out your copyright notice in the Description page of Project Settings.


#include "Tank.h"
#include "FCTween.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Input/InputDataConfig.h"

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

	CheckGround();
	
	if (PlayerController && InputEnabled() && !IsGamepadInput)
	{
		PlayerController->bShowMouseCursor = true;
		
		FHitResult CursorHitResult;
		if (PlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHitResult))
		{
			FVector HitLocation = CursorHitResult.ImpactPoint;
			HitLocation.Z = GetActorLocation().Z;
			FVector Direction = HitLocation - GetActorLocation();
			Direction.Normalize();
			RotateTurret(Direction);
		}
	}
	else if (PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
	}

	float Scale = FMath::Clamp(MoveDirection.Size(), 0.f, 1.f);
	
	if (Scale > 0.01f)
	{
		if (!IsMoving)
		{
			IsMoving = true;
			// OnStartMoving();
		}
		
		MoveDirection.Normalize();

		float Speed = Scale * MoveSpeed;
		SetActorLocation(GetActorLocation() + MoveDirection * Speed * DeltaTime, true);
		
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
			// OnStopMoving();
		}
	}

	UpdateDustParticles(IsMoving);

	// Clear move direction
	MoveDirection = FVector::ZeroVector;
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

	for (FComponentReference DustParticle : DustParticles)
	{
		UParticleSystemComponent* ParticleSystem = Cast<UParticleSystemComponent>(DustParticle.GetComponent(this));
		if (ParticleSystem)
		{
			DustParticleSystems.Add(ParticleSystem);
		}
	}
}

void ATank::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputConfig == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("InputConfig is not set in %s. Please set it in the editor."), *GetName());
		return;
	}
	
	PlayerController = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputConfig->InputMapping, 0);
	
	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(InputConfig->MoveForwardAction, ETriggerEvent::Triggered, this, &ATank::Move);
	EnhancedInput->BindAction(InputConfig->MoveRightAction, ETriggerEvent::Triggered, this, &ATank::Rotate);
	EnhancedInput->BindAction(InputConfig->FireAction, ETriggerEvent::Triggered, this, &ATank::Fire);
	EnhancedInput->BindAction(InputConfig->RotateTurretAction, ETriggerEvent::Triggered, this, &ATank::RotateTurretInDirection);
}

void ATank::Move(const FInputActionInstance& Instance)
{
	MoveDirection.X = Instance.GetValue().Get<float>();
}

void ATank::Rotate(const FInputActionInstance& Instance)
{
	MoveDirection.Y = Instance.GetValue().Get<float>();
}

void ATank::RotateTurretInDirection(const FInputActionInstance& Instance)
{
	if (!IsGamepadInput)
	{
		return;
	}

	FVector Direction = Instance.GetValue().Get<FVector>();
	
	if (Direction.SizeSquared() > 0.1f)
	{
		Direction.Set(-Direction.Y, Direction.X, 0.f);
		// Find LookAt Rotation
		FRotator TargetRotationWorld = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + Direction);
		LastTurretRotation = TargetRotationWorld;
			
		// Multiply by camera forward vector to get the correct rotation
		FRotator CameraRotation = Camera->GetComponentRotation();
		CameraRotation.Pitch = 0.f;
		CameraRotation.Roll = 0.f;
		TargetRotationWorld = UKismetMathLibrary::ComposeRotators(TargetRotationWorld, CameraRotation);

		FRotator TargetRotationLocal = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), TargetRotationWorld);
		FRotator Rotation = FMath::RInterpTo(TurretMesh->GetRelativeRotation(), TargetRotationLocal, GetWorld()->DeltaTimeSeconds, 10.f);

		TurretMesh->SetRelativeRotation(Rotation);
	}
	else
	{
		FRotator TargetRotationWorld = LastTurretRotation;
		// Multiply by camera forward vector to get the correct rotation
		FRotator CameraRotation = Camera->GetComponentRotation();
		CameraRotation.Pitch = 0.f;
		CameraRotation.Roll = 0.f;
		TargetRotationWorld = UKismetMathLibrary::ComposeRotators(TargetRotationWorld, CameraRotation);

		FRotator TargetRotationLocal = UKismetMathLibrary::InverseTransformRotation(GetActorTransform(), TargetRotationWorld);
		FRotator Rotation = FMath::RInterpTo(TurretMesh->GetRelativeRotation(), TargetRotationLocal, GetWorld()->DeltaTimeSeconds, 10.f);

		TurretMesh->SetRelativeRotation(Rotation);
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
	}, 0.125f, EFCEase::InOutSine)
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
	}, 0.375f, EFCEase::OutBounce);
}

void ATank::CheckGround()
{
	// Check if tank is on the ground
	FHitResult HitResult;
	FVector StartLocation = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	FVector EndLocation = StartLocation - FVector(0.f, 0.f, 1000.f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Raycast to check if tank is on the ground
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, QueryParams);
	if (bHit)
	{
		float DistanceToGround = FVector::Distance(GetActorLocation(), HitResult.ImpactPoint);
		if (DistanceToGround > GroundOffset + 10.f)
		{
			// Move the tank down to the ground with gravity
			FVector Location = GetActorLocation();
			float Gravity = UPhysicsSettings::Get()->DefaultGravityZ;
			Location.Z += Gravity * GravityMultiplier * GetWorld()->DeltaTimeSeconds;
			SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);
		}
		else
		{
			// Set the tank's Z location to the hit location
			FVector Location = GetActorLocation();
			Location.Z = HitResult.ImpactPoint.Z + GroundOffset;
			SetActorLocation(Location, false, nullptr, ETeleportType::TeleportPhysics);

			// Set the tank's rotation to the hit normal
			FVector Normal = HitResult.ImpactNormal;
			FVector Up = BaseMesh->GetUpVector();
			// Rotate up vector to match the hit normal
			FVector RotationAxis = FVector::CrossProduct(Up, Normal);
			float RotationAngleRad = acosf(FVector::DotProduct(Up, Normal));
			FQuat Quat = FQuat(RotationAxis, RotationAngleRad);
			FQuat NewQuat = Quat * BaseMesh->GetComponentQuat();
			FRotator NewRotator = NewQuat.Rotator();
			NewRotator.Yaw = 0;
			BaseMesh->SetRelativeRotation(NewRotator);
		}
	}
}

void ATank::UpdateDustParticles(bool Activate) const
{
	for (UParticleSystemComponent* ParticleSystem : DustParticleSystems)
	{
		if (Activate)
		{
			ParticleSystem->Activate();
		}
		else
		{
			ParticleSystem->Deactivate();
		}
	}
}