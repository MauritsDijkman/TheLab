// Fill out your copyright notice in the Description page of Project Settings.


#include "MazeCharacter.h"

// Sets default values
AMazeCharacter::AMazeCharacter()
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
	pDead = false;
	SprintSpeedMultiplier = 2.0f;
}

// Called when the game starts or when spawned
void AMazeCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the max walk speed of the character to the given max walk speed
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;

	// Add the overlap event to the function
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AMazeCharacter::OnBeginOverlap);

	// Add the UI
	if (Player_Collect_Widget_Class)
	{
		Player_Collect_Widget = CreateWidget(GetWorld(), Player_Collect_Widget_Class);
		Player_Collect_Widget->AddToViewport();
	}

	timer = startTimer;
}

// Called every frame
void AMazeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (collectedCoins >= coinsToCollect)
	{
		if (Player_Won_Widget_Class)
		{
			Player_Won_Widget = CreateWidget(GetWorld(), Player_Won_Widget_Class);
			Player_Won_Widget->AddToViewport();

			FTimerHandle localHandler;
			GetWorldTimerManager().SetTimer(localHandler, this, &AMazeCharacter::CallFadeOut_Won, 3.0f, false);
		}
	}
	else
		HandleTimer(DeltaTime);
}

// Called to bind functionality to input
void AMazeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Combine the axis to the function (executes when pressed)
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AMazeCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AMazeCharacter::MoveRight);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMazeCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMazeCharacter::StopSprinting);


	// Combine the wheel axis to the camera move function
	PlayerInputComponent->BindAxis("Wheel", this, &AMazeCharacter::MoveCamera);
}

#pragma region Movement

void AMazeCharacter::MoveForward(float Axis)
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

void AMazeCharacter::MoveRight(float Axis)
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

void AMazeCharacter::Sprint()
{
	if (!pDead)
		GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed * SprintSpeedMultiplier;
}

void AMazeCharacter::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = pMaxWalkSpeed;
}

void AMazeCharacter::MoveCamera(float Axis)
{
	if (!pDead)
	{
		if (Axis == -1 && CameraBoom->TargetArmLength < 1000.0f)
			CameraBoom->TargetArmLength += 20.0f;
		else if (Axis == 1 && CameraBoom->TargetArmLength > 200.0f)
			CameraBoom->TargetArmLength -= 20.0f;
	}
}

#pragma endregion

#pragma region Overlap

void AMazeCharacter::OnBeginOverlap(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Coin"))
	{
		collectedCoins++;
		OtherActor->Destroy();

		// Spawn particle
		if (PickingUpCoinEffect)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickingUpCoinEffect, GetActorLocation());
	}
}

void AMazeCharacter::OnEndOverlap(class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor, class UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{

}

#pragma endregion

#pragma region Handler

void AMazeCharacter::HandleTimer(float pMinusAmount)
{
	if (!pDead)
	{
		if (timer > 0)
			timer -= pMinusAmount;
		else if (timer <= 0)
		{
			timer = 0;

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
				GetWorldTimerManager().SetTimer(localHandler, this, &AMazeCharacter::CallFadeOut_Lost, 3.0f, false);
			}
		}
	}
}

#pragma endregion

#pragma region Callers / Level Switchers / Data Savers

void AMazeCharacter::CallFadeOut_Won()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);

	level2Won = true;
	CallSaveGameVariables();

	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &AMazeCharacter::ToMainLevel, 0.83f, false);
}

void AMazeCharacter::CallFadeOut_Lost()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SetFadeOut true"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);

	FTimerHandle localHandler;
	GetWorldTimerManager().SetTimer(localHandler, this, &AMazeCharacter::RestartGame, 0.83f, false);
}

void AMazeCharacter::ToMainLevel()
{
	UGameplayStatics::OpenLevel(this, "ThirdPersonMap_2");
}

void AMazeCharacter::RestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void AMazeCharacter::CallSaveGameVariables()
{
	FOutputDeviceNull argument;
	const FString command = FString::Printf(TEXT("SaveGame Slot1"));

	if (blueprintActor)
		blueprintActor->CallFunctionByNameWithArguments(*command, argument, NULL, true);
}

#pragma endregion
