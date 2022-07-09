// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include <string>


// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the size of the capsule
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// Turns off the rotattion of the player (so that only camera moves)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;									// Allows the character movement to rotate in the direction it is moving
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f) * GetActorScale3D().X;	// The rate that is used for the rotation
	GetCharacterMovement()->JumpZVelocity = 600.0f;												// Velocity when the player jumps
	GetCharacterMovement()->AirControl = 0.2f;													// Control of the player in the air

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));				// Create cameraboom (springarm)
	CameraBoom->SetupAttachment(RootComponent);													// Set it as a child of the capsule

	CameraBoom->TargetArmLength = CameraBoom->TargetArmLength;									// How far the spring arm (camera) will be from the player
	CameraBoom->bUsePawnControlRotation = true;													// Rotate the arm based on the controller

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));				// Create camera
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);					// Link the camera to the end of the springboom
	FollowCamera->bUsePawnControlRotation = false;												// The camera doesn't rotate relative to the arm

	// Set variables
	SprintSpeedMultiplier = 2.0f;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the max walk speed of the character to the given max walk speed
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;

	// Add the overlap event to the function
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlap);

	// Set variables
	level1UIActive = false;
	level2UIActive = false;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Link the different inputs to the player input
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	// Combine the axis to the function (executes when pressed)
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprinting);


	// Combine the start level button to the function
	PlayerInputComponent->BindAction("StartLevel", IE_Pressed, this, &APlayerCharacter::HandleGameStart);

	// Combine the wheel axis to the camera move function
	PlayerInputComponent->BindAxis("Wheel", this, &APlayerCharacter::MoveCamera);
}

#pragma region Movement

void APlayerCharacter::MoveForward(float Axis)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Calculate the forward direction vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Axis);

}

void APlayerCharacter::MoveRight(float Axis)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Calculate the right direction vector
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void APlayerCharacter::MoveCamera(float Axis)
{
	if (Axis == -1 && CameraBoom->TargetArmLength < 1000.0f)
		CameraBoom->TargetArmLength += 20.0f;
	else if (Axis == 1 && CameraBoom->TargetArmLength > 200.0f)
		CameraBoom->TargetArmLength -= 20.0f;
}

void APlayerCharacter::Sprint()
{
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed * SprintSpeedMultiplier;
}

void APlayerCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;
}

#pragma endregion

#pragma region Overlap

void APlayerCharacter::OnBeginOverlap(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("End"))
		CallFadeOutForEnd();
	else if (OtherActor->ActorHasTag("Game1") && !level1Won)
	{
		if (Player_Level_Widget_Class && !level1UIActive)
		{
			// Add the UI
			Player_Level_Widget = CreateWidget(GetWorld(), Player_Level_Widget_Class);
			Player_Level_Widget->AddToViewport();

			// Set level bool
			level1UIActive = true;
		}
	}
	else if (OtherActor->ActorHasTag("Game2") && !level2Won)
	{
		if (Player_Level_Widget_Class && !level2UIActive)
		{
			// Add the UI
			Player_Level_Widget = CreateWidget(GetWorld(), Player_Level_Widget_Class);
			Player_Level_Widget->AddToViewport();

			// Set level bool
			level2UIActive = true;
		}
	}
}

void APlayerCharacter::OnEndOverlap(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Remove the specific level UI and set booleans
	if (Player_Level_Widget_Class && level1UIActive && OtherActor->ActorHasTag("Game1"))
	{
		Player_Level_Widget->RemoveFromViewport();
		level1UIActive = false;
	}
	else if (Player_Level_Widget_Class && level2UIActive && OtherActor->ActorHasTag("Game2"))
	{
		Player_Level_Widget->RemoveFromViewport();
		level2UIActive = false;
	}
}

#pragma endregion

#pragma region Callers / Level Switchers / Data Savers

void APlayerCharacter::CallFadeOutForEnd()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);

	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &APlayerCharacter::LoadEndScene, 0.83f, false);
}

void APlayerCharacter::LoadEndScene()
{
	UGameplayStatics::OpenLevel(this, "EndMenu");
}

void APlayerCharacter::HandleGameStart()
{
	if (level1UIActive && !level2UIActive)
	{
		CallFadeOutEvent();
		GetWorld()->GetTimerManager().SetTimer(loopTimerHandler, this, &APlayerCharacter::OnTimerEndGame1, 0.83f, false);
	}
	else if (level2UIActive && !level1UIActive)
	{
		CallFadeOutEvent();
		GetWorld()->GetTimerManager().SetTimer(loopTimerHandler, this, &APlayerCharacter::OnTimerEndGame2, 0.83f, false);
	}
}

void APlayerCharacter::OnTimerEndGame1()
{
	UGameplayStatics::OpenLevel(this, "Game1");
}

void APlayerCharacter::OnTimerEndGame2()
{
	UGameplayStatics::OpenLevel(this, "Game2");
}

void APlayerCharacter::CallFadeOutEvent()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);
}

void APlayerCharacter::CallSaveGameVariables()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SaveGame Slot1"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);
}

#pragma endregion
