// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "Camera/CameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Components/SphereComponent.h"

#include "Animation/SkeletalMeshActor.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Blueprint/UserWidget.h"

#include "Misc/OutputDeviceNull.h"

#include "CollectCharacter.generated.h"

UCLASS()
class PP_TERM4_API ACollectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACollectCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	// Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		UCameraComponent* FollowCamera;


	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking")
		float pMaxWalkSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Movement: Walking")
		float SprintSpeedMultiplier;


	// Winning states
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool level1Won;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool level2Won;


	// Health variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float Health;

	UPROPERTY(EditAnywhere)
		float HealthDecreaseAmount;


	// Timer
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Timer")
		float timer = 30.0f;


	// UI references	
	UPROPERTY(EditAnyWhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Player_Health_Widget_Class;
	UUserWidget* Player_Health_Widget;

	UPROPERTY(EditAnyWhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Player_Won_Widget_Class;
	UUserWidget* Player_Won_Widget;

	UPROPERTY(EditAnyWhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Player_Lost_Widget_Class;
	UUserWidget* Player_Lost_Widget;


	// Particle reference
	UPROPERTY(EditAnyWhere, Category = "Particles")
		UParticleSystem* PickingUpHealthEffect;


	// Blueprint reference
	UPROPERTY(EditAnywhere, Category = "BlueprintClass")
		AActor* blueprintActor;

private:
	// Movement
	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void MoveCamera(float Axis);

	void Sprint();
	void StopSprinting();


	// Handlers
	void HandleTimer(float pMinusAmount);
	void HandleHP(float pDeltaTime);


	// Callers / Level Switchers / Data Savers
	void CallFadeOut_Won();
	void CallFadeOut_Lost();

	void ToMainLevel();
	void RestartGame();

	void CallSaveGameVariables();


	// Dead
	bool pDead;


	// Overlap
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* HitComponent,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComponent,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
};
