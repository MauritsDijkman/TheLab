// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectCharacter.h"

// Sets default values
ACollectCharacter::ACollectCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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
	pDead = false;
	SprintSpeedMultiplier = 2.0f;
	Health = 100.0f;
	HealthDecreaseAmount = 5.0f;
}

// Called when the game starts or when spawned
void ACollectCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the max walk speed of the character to the given max walk speed
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;

	// Add the overlap event to the function
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACollectCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &ACollectCharacter::OnEndOverlap);

	// Add the UI
	if (Player_Health_Widget_Class)
	{
		Player_Health_Widget = CreateWidget(GetWorld(), Player_Health_Widget_Class);
		Player_Health_Widget->AddToViewport();
	}
}

// Called every frame
void ACollectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleTimer(DeltaTime);
	HandleHP(DeltaTime);
}

// Called to bind functionality to input
void ACollectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Link the different inputs to the player input
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);


	// Combine the axis to the function (executes when pressed)
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACollectCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACollectCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ACollectCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ACollectCharacter::StopSprinting);


	// Combine the wheel axis to the camera move function
	PlayerInputComponent->BindAxis("Wheel", this, &ACollectCharacter::MoveCamera);
}

#pragma region Movement

void ACollectCharacter::MoveForward(float Axis)
{
	if (!pDead)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Calculate the forward direction vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);
	}
}

void ACollectCharacter::MoveRight(float Axis)
{
	if (!pDead)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Calculate the right direction vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Axis);
	}
}

void ACollectCharacter::MoveCamera(float Axis)
{
	if (!pDead)
	{
		if (Axis == -1 && CameraBoom->TargetArmLength < 1000.0f)
			CameraBoom->TargetArmLength += 20.0f;
		else if (Axis == 1 && CameraBoom->TargetArmLength > 200.0f)
			CameraBoom->TargetArmLength -= 20.0f;
	}
}

void ACollectCharacter::Sprint()
{
	if (!pDead)
		GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed * SprintSpeedMultiplier;
}

void ACollectCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;
}

#pragma endregion

#pragma region Overlap

void ACollectCharacter::OnBeginOverlap(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Recharge"))
	{
		Health += 10.0f;

		if (Health > 100.0f)
			Health = 100.0f;

		OtherActor->Destroy();

		// Spawn particle
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickingUpHealthEffect, GetActorLocation());
	}
}

void ACollectCharacter::OnEndOverlap(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

}

#pragma endregion

#pragma region Handlers

void ACollectCharacter::HandleTimer(float pMinusAmount)
{
	if (!pDead)
	{
		if (timer > 0)
			timer -= pMinusAmount;
		else if (timer <= 0)
		{
			timer = 0;

			if (Player_Won_Widget_Class)
			{
				Player_Won_Widget = CreateWidget(GetWorld(), Player_Won_Widget_Class);
				Player_Won_Widget->AddToViewport();
			}

			FTimerHandle localHandler;
			GetWorldTimerManager().SetTimer(localHandler, this, &ACollectCharacter::CallFadeOut_Won, 3.0f, false);
		}
	}
}

void ACollectCharacter::HandleHP(float pDeltaTime)
{
	if (timer > 0)
		Health -= pDeltaTime * HealthDecreaseAmount;

	if (Health <= 0)
	{
		if (!pDead)
		{
			pDead = true;
			GetMesh()->SetSimulatePhysics(true);

			if (Player_Lost_Widget_Class)
			{
				Player_Lost_Widget = CreateWidget(GetWorld(), Player_Lost_Widget_Class);
				Player_Lost_Widget->AddToViewport();
			}

			FTimerHandle localHandler;
			GetWorldTimerManager().SetTimer(localHandler, this, &ACollectCharacter::CallFadeOut_Lost, 3.0f, false);
		}
	}
}

#pragma endregion

#pragma region Callers / Level Switchers / Data Savers

void ACollectCharacter::CallFadeOut_Won()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);

	level1Won = true;
	CallSaveGameVariables();

	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &ACollectCharacter::ToMainLevel, 0.83f, false);
}

void ACollectCharacter::CallFadeOut_Lost()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);

	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &ACollectCharacter::RestartGame, 0.83f, false);
}

void ACollectCharacter::ToMainLevel()
{
	UGameplayStatics::OpenLevel(this, "ThirdPersonMap_2");
}

void ACollectCharacter::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void ACollectCharacter::CallSaveGameVariables()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SaveGame Slot1"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);
}

#pragma endregion
