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

#include "PlayerCharacter.generated.h"

UCLASS()
class PP_TERM4_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

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

	// UI references	
	UPROPERTY(EditAnyWhere, Category = "UI HUD")
		TSubclassOf<UUserWidget> Player_Level_Widget_Class;
	UUserWidget* Player_Level_Widget;

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


	// Handler
	void HandleGameStart();


	// Callers / Level Switchers / Data Savers
	void CallFadeOutEvent();
	void CallSaveGameVariables();

	void CallFadeOutForEnd();
	void LoadEndScene();

	void OnTimerEndGame1();
	void OnTimerEndGame2();


	// UI active state
	bool level1UIActive;
	bool level2UIActive;


	// Timer
	FTimerHandle loopTimerHandler;


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
