// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerCharacter_GameMode.generated.h"

/**
 *
 */
UCLASS()
class PP_TERM4_API APlayerCharacter_GameMode : public AGameMode
{
	GENERATED_BODY()

		APlayerCharacter_GameMode();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// Object to spawn
	UPROPERTY(EditAnywhere, Category = "Spawn Object")
		TSubclassOf<APawn> PlayerRecharge;

	// Coordinates
	UPROPERTY(EditAnywhere, Category = "Spawn Coordinates")
		float Spawn_Z = 270.0f;

	UPROPERTY(EditAnywhere, Category = "Spawn Coordinates")
		float Spawn_X_Min;

	UPROPERTY(EditAnywhere, Category = "Spawn Coordinates")
		float Spawn_X_Max;

	UPROPERTY(EditAnywhere, Category = "Spawn Coordinates")
		float Spawn_Y_Min;

	UPROPERTY(EditAnywhere, Category = "Spawn Coordinates")
		float Spawn_Y_Max;

private:
	void SpawnPlayerRecharge();
};
