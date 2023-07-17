// Fill out your copyright notice in the Description page of Project Settings.


#include "TankBase.h"
#include "FCTween.h"
#include "HealthComponent.h"
#include "Projectile.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATankBase::ATankBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Mesh"));
	BaseMesh->SetupAttachment(RootComponent);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turret Mesh"));
	TurretMesh->SetupAttachment(BaseMesh);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(TurretMesh);
}

// Called when the game starts or when spawned
void ATankBase::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent = FindComponentByClass<UHealthComponent>();
	if (HealthComponent)
	{
		OnTakeAnyDamage.AddDynamic(HealthComponent, &UHealthComponent::DamageTaken);
	}
}

void ATankBase::RotateTurret(FVector LookAtTarget)
{
	FRotator TargetRotation = FRotator(0.f, LookAtTarget.Rotation().Yaw, 0.f);
	FRotator Rotation = FMath::RInterpTo(TurretMesh->GetComponentRotation(), TargetRotation, GetWorld()->DeltaTimeSeconds, 10.f);
	TurretMesh->SetWorldRotation(Rotation);
}

void ATankBase::Fire()
{
	FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
	FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();

	// DrawDebugSphere(GetWorld(), SpawnLocation, 25.f, 8, FColor::Red, false, 2.f);

	AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	Projectile->SetOwner(this);
}

// Called every frame
void ATankBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATankBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ATankBase::OnDamageTaken()
{
	// Animate the scale of the tank when it takes damage
	BaseMesh->SetWorldScale3D(FVector(1.25f));
	FCTween::Play(1.2f, 1.f, [this](float Value)
	{
		BaseMesh->SetWorldScale3D(FVector(Value));
	}, 0.3f, EFCEase::OutBounce);
}

void ATankBase::HandleDestruction()
{
	if (DestructionParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, DestructionParticle, GetActorLocation());
	}

	if (DestructionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DestructionSound, GetActorLocation());
	}
}

