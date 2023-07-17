// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "TankBase.h"
#include "TanksGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	GameMode = Cast<ATanksGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health);

	if (Health <= 0.f)
	{
		if (GameMode)
		{
			GameMode->ActorDied(DamagedActor);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Health component has no reference to the game mode"));
		}
	}
	else
	{
		if (ATankBase* Tank = Cast<ATankBase>(DamagedActor))
		{
			Tank->OnDamageTaken();
		}
	}
}

