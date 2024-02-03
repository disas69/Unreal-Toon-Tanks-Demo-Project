// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputDataConfig.generated.h"

class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class TOONTANKS_API UInputDataConfig : public UDataAsset
{
	GENERATED_BODY()

public:
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
};
