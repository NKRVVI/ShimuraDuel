// Fill out your copyright notice in the Description page of Project Settings.


#include "LordShimura.h"

#include "Characters/ShimuraPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ALordShimura::ALordShimura()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
}

// Called when the game starts or when spawned
void ALordShimura::BeginPlay()
{
	Super::BeginPlay();

	if (AShimuraPlayer* Player = Cast<AShimuraPlayer>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		Opponent = Player;
	}
}

void ALordShimura::OnOpponentDodgeFinished_Implementation()
{
	Opponent->OnDodgeFinished.RemoveAll(this);

	GetMesh()->GetAnimInstance()->Montage_Resume(AttackMontage);
}

// Called every frame
void ALordShimura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState != EActionState::Attack || !Opponent->IsDodging())
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Opponent->GetActorLocation());
		FRotator ControllerRotation = FMath::Lerp<FRotator, float>(Controller->GetControlRotation(), LookAtRotation, .9f);
		Controller->SetControlRotation(LookAtRotation);
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw = Controller->GetControlRotation().Yaw;
		SetActorRotation(Rotation);
	}

	Attack();
	
}

// Called to bind functionality to input
void ALordShimura::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

ACombatCharacter* ALordShimura::GetOpponent()
{
	return Opponent;
}

