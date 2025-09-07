// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LordShimura.h"

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

	//when the player has started an attack, this function will be called. This is used to parry when the player is too agressive and counterattack
	Opponent->OnActionStarted.AddUniqueDynamic(this, &ThisClass::OnOpponentAttackStarted);

}

/*
 * this is the similar to the base function and also caches the result of the last attack
 */
void ALordShimura::FinishAttacking_Implementation()
{
	Super::FinishAttacking_Implementation();
	AttackEnd = Success;
}

/*
 *	this function is similar to the base function along with the following tasks:
 *	it broadcasts that its previous attack has been interuppted by another attack
 *	it bind to the attackfinished delegate that the player has, this is used to calculate the time taken between the end of an attack and the starting of the new one
 *	if they are too close, then the player is starting to get aggressive
 */
void ALordShimura::GetHit_Implementation(float Damage, FVector ImpactPoint)
{
	Super::GetHit_Implementation(Damage, ImpactPoint);
	OnAttackFinished.Broadcast(EAttackEndType::GetHit);
	AttackEnd = EAttackEndType::GetHit;

	Opponent->OnAttackFinished.AddUniqueDynamic(this, &ThisClass::OnOpponentAttackFinished);
}

/*
 * this function caches the time at which the attack has ended
 */
void ALordShimura::OnOpponentAttackFinished_Implementation(EAttackEndType CurrentAttackEnd)
{
	//found potential bug, does not account for distance
	GetHitTime = GetWorld()->GetTimeSeconds();
}

/*
 *	this function is called whenever an attack by the opponent has started.
 *	SHimura decides whether to dodge the attack or not. He also calculate the average time between the last three attacks of the player
 *	if the average is too small, then the player is getting aggressive and a parry is scheduled. the class binds to the BecomeParrible delegate of the
 *	opponent and if he is close enough, he will be parried
 */
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

/*
 *	this function is similar to the base function other than cacheing the result of the attack
 */
void ALordShimura::GetParried_Implementation()
{
	Super::GetParried_Implementation();
	OnAttackFinished.Broadcast(EAttackEndType::Parried);
	AttackEnd = Parried;
}

/*
 *	upon parrying the player, the variables are reset and we wait until they get aggressive again
 */
void ALordShimura::Parry_Implementation()
{
	Super::Parry_Implementation();
	bReadyForParry = false;
	AttackIntervals.Empty();
	Opponent->OnBecomeParriable.RemoveAll(this);
}

void ALordShimura::OnDead_Implementation()
{
	Super::OnDead_Implementation();

	Opponent->OnBecomeParriable.RemoveAll(this);
	Opponent->OnAttackFinished.RemoveAll(this);
	Opponent->OnActionStarted.RemoveAll(this);
}

/*
 *	in tick we make sure that face the opponent at all time except when we are dodging and attacking
 */
void ALordShimura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;
	if (CurrentState != EActionState::Attack || !Opponent->IsDodging())
	{
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Opponent->GetActorLocation());
		FRotator Rotation = GetActorRotation();
		Rotation.Yaw = LookAtRotation.Yaw;
		SetActorRotation(Rotation);
	}
}

ACombatCharacter* ALordShimura::GetOpponent() const
{
	return Opponent;
}