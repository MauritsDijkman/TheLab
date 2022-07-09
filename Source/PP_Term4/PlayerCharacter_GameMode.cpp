// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter_GameMode.h"
#include "GameFramework/Actor.h"

APlayerCharacter_GameMode::APlayerCharacter_GameMode()
{
	PrimaryActorTick.bCanEverTick = true;	// Calls the tick function every frame every second
}

void APlayerCharacter_GameMode::BeginPlay()
{
	Super::BeginPlay();

	// Call the function evert 2 till 5 seconds
	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &APlayerCharacter_GameMode::SpawnPlayerRecharge, FMath::RandRange(2, 5), true);
}

void APlayerCharacter_GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter_GameMode::SpawnPlayerRecharge()
{
	// Get random coordinates
	float RandX = FMath::RandRange(Spawn_X_Min, Spawn_X_Max);
	float RandY = FMath::RandRange(Spawn_Y_Min, Spawn_Y_Max);

	// Create spawn position and rotaion
	FVector SpawnPosition = FVector(RandX, RandY, Spawn_Z);
	FRotator SpawnRotation = FRotator(0.0f, 0.0f, 0.0f);

	// Spawn object with given position and rotation
	GetWorld()->SpawnActor(PlayerRecharge, &SpawnPosition, &SpawnRotation);
}
