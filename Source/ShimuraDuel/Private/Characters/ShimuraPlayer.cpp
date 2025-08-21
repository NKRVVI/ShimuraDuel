// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShimuraDuel/Public/Characters/ShimuraPlayer.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "LordShimura.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AShimuraPlayer

AShimuraPlayer::AShimuraPlayer()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AShimuraPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (ALordShimura* Lord = Cast<ALordShimura>(UGameplayStatics::GetActorOfClass(this, ALordShimura::StaticClass())))
	{
		Opponent = Lord;
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AShimuraPlayer::FinishDodging()
{
	bDodging = false;
	OnDodgeFinished.Broadcast();
}

void AShimuraPlayer::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShimuraPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShimuraPlayer::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShimuraPlayer::Look);

		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Completed, this, &ThisClass::Dodge);

		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ThisClass::BlockStart);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ThisClass::BlockEnd);
		EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Triggered, this, &ThisClass::Parry);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShimuraPlayer::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	MovementVector = Value.Get<FVector2D>();

	if (bDodging) return;
	
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShimuraPlayer::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AShimuraPlayer::Dodge(const FInputActionValue& Value)
{
	if (bDodging) return;
	
	if (MovementVector.Equals(FVector2D::Zero()))
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(DodgeMontage);
	
	if (FMath::IsNearlyZero(MovementVector.X))
	{
		if (MovementVector.Y > 0.f)
		{
			AnimInstance->Montage_JumpToSection(FName("Front"), DodgeMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FName("Back"), DodgeMontage);
		}
	}
	else
	{
		if (MovementVector.X > 0.f)
		{
			AnimInstance->Montage_JumpToSection(FName("Right"), DodgeMontage);
		}
		else
		{
			AnimInstance->Montage_JumpToSection(FName("Left"), DodgeMontage);
		}
	}

	bDodging = true;
}

void AShimuraPlayer::BlockStart(const FInputActionValue& Value)
{
	bIsBlocking = true;
	UE_LOG(LogTemp, Warning, TEXT("BlockStart"));
}

void AShimuraPlayer::BlockEnd(const FInputActionValue& Value)
{
	bIsBlocking = false;
	UE_LOG(LogTemp, Warning, TEXT("BlockEnd"));
}

void AShimuraPlayer::Parry(const FInputActionValue& Value)
{
	ACombatCharacter::Parry();
	UE_LOG(LogTemp, Warning, TEXT("Parry"));
}

void AShimuraPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Controller->SetControlRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Opponent->GetActorLocation()));
	FRotator Rotation = GetActorRotation();
	Rotation.Yaw = Controller->GetControlRotation().Yaw;
	SetActorRotation(Rotation);
}
