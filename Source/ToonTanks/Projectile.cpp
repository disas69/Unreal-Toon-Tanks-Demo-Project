// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsActive)
	{
		return;
	}
	
	AActor* OwnerActor = GetOwner();
	if (OwnerActor && OtherActor && OtherActor != OwnerActor && OtherActor != this)
	{
		IsActive = false;
		
		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			OwnerActor->GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

		if (HitParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				HitParticle,
				GetActorLocation()
			);
		}

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				GetActorLocation()
			);
		}

		// Deactivate actor and destroy it after 1 second
		ProjectileMesh->SetVisibility(false, true);
		ProjectileMesh->SetSimulatePhysics(false);
		ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProjectileMovement->Deactivate();
		SetLifeSpan(1.f);
	}
}
