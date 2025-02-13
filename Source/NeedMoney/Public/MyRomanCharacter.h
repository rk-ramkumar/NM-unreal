// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"		
#include "MyRomanCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAnimSequence;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UENUM(BlueprintType)
enum class EAnimationState : uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	RUNNING UMETA(DisplayName = "RUNNING"),
	FALLING UMETA(DisplayName = "FALLING"),
	LANDING UMETA(DisplayName = "LANDING"),
	WALKING UMETA(DisplayName = "WALKING"),
	JUMPING UMETA(DisplayName = "JUMPING")
};
UCLASS(config=Game)


class AMyRomanCharacter : public ACharacter
{
	GENERATED_BODY()

	// Camera boom positioning the camera behind the character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	// MappingContext
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	// Jump Input Action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta= (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	// Move Input Action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta= (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// Look Input Action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta= (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	// Crouch Input Action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	// Sprint Input Action 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta= (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	EAnimationState CurrentAnimation;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* IdleAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* RunAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* FallAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* LandAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* WalkAnim;

	UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimSequence* JumpAnim;

public:
	// Sets default values for this character's properties
	AMyRomanCharacter();

protected:

	// Called for movement Input 
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void BeginSprint(const FInputActionValue& Value);

	void EndSprint(const FInputActionValue& Value);

	void crouch(const FInputActionValue& Value);

	void unCrouch(const FInputActionValue& Value);

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	void UpdateAnimation();

	void SetupAnimationMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	
	void Landed(const FHitResult& Hit) override;

	void Jump() override;

	void StopJumping() override;

	void PlayAnimation(UAnimationAsset* Animation, const bool loop);
};
