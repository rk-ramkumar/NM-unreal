// Fill out your copyright notice in the Description page of Project Settings.

#include "RomanCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhnacedInputSubSystem"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"


// Sets default values
ARomanCharacter::ARomanCharacter() {
  // Set size for collision capsule
  GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  GetCharacterMovement()->bOrientRotationToMovement = true;
  GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
  GetCharacterMovement()->JumpZVelocity = 700.0f;
  GetCharacterMovement()->AirControl = 0.35f;
  GetCharacterMovement()->MaxWalkSpeed = 500.f;
  GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
  GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
  GetcharacterMovement()->BrakingDecelerationFalling = 1500.f;

  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength = 400.f;
  CameraBoom->bUsePawnControlRotation = true;

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;
}

// Called when the game starts or when spawned
void ARomanCharacter::BeginPlay() {
  Super::BeginPlay();

  if (APlayerController* PlayerController  =
          Cast<APlayerController>(Controller)) {

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
            ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
                PlayerController->GetLocalPlayer())) {
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
  }
}

// Called to bind functionality to input
void ARomanCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent) 
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ARomanCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARomanCharacter::Look);
	}
	else {
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
	}
}

void ANeedMoneyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != null)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANeedMoneyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(Controller != null)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.x);
		AddControllerPitchInput(LookAxisVector.y);
	}
}