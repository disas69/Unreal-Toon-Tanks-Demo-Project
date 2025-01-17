// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TanksGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TOONTANKS_API ATanksGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	class ATank* PlayerTank;
	class ATankPlayerController* PlayerController;
	int32 TargetTurretsCount = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	void ActorDied(AActor* DeadActor);

	UPROPERTY(EditAnywhere)
	float StartDelay = 3.f;

	UFUNCTION(BlueprintImplementableEvent)
	void StartGame();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOver(bool WonGame);

	void HandleGameStart();
	int32 GetTargetTurretsCount();
	
};
