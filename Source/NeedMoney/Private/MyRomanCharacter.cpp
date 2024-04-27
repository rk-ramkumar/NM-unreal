#include "MyRomanCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Animation/AnimSequence.h" // Include for UAnimSequence
#include "UObject/ConstructorHelpers.h"
#include "Animation/SkeletalMeshActor.h"


// Sets default values
AMyRomanCharacter::AMyRomanCharacter() {
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
  GetCharacterMovement()->BrakingDecelerationFalling = 1500.f;

  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->TargetArmLength = 400.f;
  CameraBoom->bUsePawnControlRotation = true;

  FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
  FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  FollowCamera->bUsePawnControlRotation = false;
  
  CurrentAnimation = EAnimationState::IDLE;
}

// Called when the game starts or when spawned
void AMyRomanCharacter::BeginPlay() {
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
void AMyRomanCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMyRomanCharacter::BeginSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMyRomanCharacter::EndSprint);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyRomanCharacter::crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMyRomanCharacter::unCrouch);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyRomanCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyRomanCharacter::Look);
	}
	else {
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
	}
}

void AMyRomanCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

	}
}

void AMyRomanCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if(Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMyRomanCharacter::BeginSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 2000.0f;
	CurrentAnimation = EAnimationState::RUNNING;
	AMyRomanCharacter::PlayAnimation(RunAnim, true);

}

void AMyRomanCharacter::EndSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;
	CurrentAnimation = EAnimationState::WALKING;
	AMyRomanCharacter::UpdateAnimation();

}

void AMyRomanCharacter::crouch(const FInputActionValue& Value)
{
	Crouch();
}

void AMyRomanCharacter::unCrouch(const FInputActionValue& Value)
{
	UnCrouch();
	CurrentAnimation = EAnimationState::IDLE;
}

void AMyRomanCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	CurrentAnimation = EAnimationState::LANDING;
	AMyRomanCharacter::PlayAnimation(LandAnim, false);
}

void AMyRomanCharacter::Jump()
{
	Super::Jump();
	if (!GetCharacterMovement()->IsFalling()){
		CurrentAnimation = EAnimationState::JUMPING;
		AMyRomanCharacter::PlayAnimation(JumpAnim, false);
	}

}

void AMyRomanCharacter::StopJumping()
{
	Super::StopJumping();
	CurrentAnimation = EAnimationState::FALLING;
}

void AMyRomanCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
    UpdateAnimation();
}

void AMyRomanCharacter::UpdateAnimation()
{
	UAnimSequence* CharacterAnimation = nullptr;

	switch (GetCharacterMovement()->MovementMode)
    {
    case MOVE_Walking:
		if (FMath::IsNearlyZero(GetVelocity().Size()))
        {
            CurrentAnimation = EAnimationState::IDLE;
            CharacterAnimation = IdleAnim;
        }
        else
        {
            CurrentAnimation = EAnimationState::WALKING;
            CharacterAnimation = WalkAnim;
        }
		AMyRomanCharacter::PlayAnimation(CharacterAnimation, true);
        break;
    case MOVE_Falling:
        CurrentAnimation = EAnimationState::FALLING;
		AMyRomanCharacter::PlayAnimation(FallAnim, true);
        break;
    default:
        CurrentAnimation = EAnimationState::IDLE;
		AMyRomanCharacter::PlayAnimation(IdleAnim, true);
        break;
    }

}

void AMyRomanCharacter::PlayAnimation(UAnimationAsset* Animation, const bool loop)
{
	if (USkeletalMeshComponent* mesh = AMyRomanCharacter::GetMesh())
	{
		mesh->PlayAnimation(Animation, loop);
	}
}
