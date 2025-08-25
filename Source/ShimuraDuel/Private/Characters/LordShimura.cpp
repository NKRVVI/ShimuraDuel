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

	Opponent->OnActionStarted.AddUniqueDynamic(this, &ThisClass::OnOpponentAttackStarted);

}

void ALordShimura::FinishAttacking_Implementation()
{
	Super::FinishAttacking_Implementation();
	AttackEnd = Success;
}

void ALordShimura::GetHit_Implementation(float Damage, FVector ImpactPoint)
{
	Super::GetHit_Implementation(Damage, ImpactPoint);
	OnAttackFinished.Broadcast(EAttackEndType::GetHit);
	AttackEnd = EAttackEndType::GetHit;

	Opponent->OnAttackFinished.AddUniqueDynamic(this, &ThisClass::OnOpponentAttackFinished);
}

void ALordShimura::OnOpponentAttackFinished_Implementation(EAttackEndType CurrentAttackEnd)
{
	GetHitTime = GetWorld()->GetTimeSeconds();
}

void ALordShimura::OnOpponentAttackStarted_Implementation(EActionState ActionState)
{
	if (ActionState == EActionState::Attack)
	{
		if (AttackEnd == Success)
		{
			if (FMath::RandRange(0.f, 1.f) < 0.15f)
			{
				Dodge();
				AttackIntervals.Empty();
				Opponent->OnBecomeParriable.RemoveAll(this);
				return;
			}
		}

		if ((Opponent->GetActorLocation() - GetActorLocation()).Size2D() < 250.f)
		{
			NextAttackTime = GetWorld()->GetTimeSeconds();
			AttackIntervals.Add(NextAttackTime - GetHitTime);

			while (AttackIntervals.Num() > 2)
			{
				AttackIntervals.RemoveAt(0);
			}
			
			if (AttackIntervals.Num() == 2)
			{
				float AverageTime = 0.f;
				for (float Interval : AttackIntervals)
				{
					AverageTime += Interval;
				}
				AverageTime /= AttackIntervals.Num();

				if (AverageTime < 0.25f)
				{
					bReadyForParry = true;
					Opponent->OnBecomeParriable.AddDynamic(this, &ThisClass::Parry);
				}
			}
		}
	}
}

void ALordShimura::GetParried_Implementation()
{
	Super::GetParried_Implementation();
	OnAttackFinished.Broadcast(EAttackEndType::Parried);
	AttackEnd = Parried;
}

void ALordShimura::Parry_Implementation()
{
	Super::Parry_Implementation();
	bReadyForParry = false;
	AttackIntervals.Empty();
	Opponent->OnBecomeParriable.RemoveAll(this);
}

// Called every frame
void ALordShimura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState != EActionState::Attack || !Opponent->IsDodging())
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Opponent->GetActorLocation());
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw = LookAtRotation.Yaw;
		SetActorRotation(Rotation);
	}
}

// Called to bind functionality to input
void ALordShimura::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

ACombatCharacter* ALordShimura::GetOpponent() const
{
	return Opponent;
}