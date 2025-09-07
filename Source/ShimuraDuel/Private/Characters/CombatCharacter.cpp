// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CombatCharacter.h"

#include "AttributeComponent.h"
#include "Katana.h"
#include "ShimuraDuelGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


ACombatCharacter::ACombatCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Katana = CreateDefaultSubobject<UChildActorComponent>("Katana");
	Katana->SetupAttachment(GetMesh(), FName("katana_r"));

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>("Attributes");
}

/*
 *	this function is called when an attack is received while the character is blocking. It takes into accoung the stagger damage of the attack to know whether our posture
 *	has been broken or not
 */
void ACombatCharacter::GetStaggerHit_Implementation(float Damage)
{
	AttributeComponent->AddOrRemovePosture(-Damage);
	if (AttributeComponent->GetPosture() <= 0)
	{
		GetHit(AttributeComponent->GetMaxHealth() / 2.f, FVector(0, 0, 0));
		AttributeComponent->AddOrRemovePosture(100.f);
		CurrentState = EActionState::None;
		if (GetOpponent()->bInImpactWindow)
		{
			PlayCameraShake();
		}
	}
	else
	{
		PlayRandomMontageSection(StaggerHitMontage);
	}
}

/*
 *	this function plays the parry spark + time dilation effect
 */
void ACombatCharacter::PlayParryEffect_Implementation()
{
	if (!bPlayParryEffect) return;
	
	if (AKatana* Sword = Cast<AKatana>(Katana->GetChildActor()))
	{
		Sword->PlayParryEffect();
	}
	UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
}

/*
 *	this function gets the current animation playing in the current montage that is active. This is used to find out what attack is playing so that we can
 *	check the damage to give to the opponent
 */
UAnimationAsset* ACombatCharacter::GetCurrentAttackAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance) return nullptr;

	// Grab the currently active montage instance
	FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance();
	if (!MontageInstance || !MontageInstance->Montage) return nullptr;

	UAnimMontage* Montage = MontageInstance->Montage;
	float CurrentPosition = MontageInstance->GetPosition();
	
	// Iterate over all slots in the montage
	for (const FSlotAnimationTrack& SlotTrack : Montage->SlotAnimTracks)
	{
		// Each slot has an AnimTrack, which is made up of segments
		for (const FAnimSegment& Segment : SlotTrack.AnimTrack.AnimSegments)
		{
			if (CurrentPosition >= Segment.StartPos && CurrentPosition < Segment.GetEndPos())
			{
				return Segment.GetAnimReference(); // <-- This is the actual animation currently being played
			}
		}
	}

	return nullptr;
}

/*
 *	this function dodges backwards. This is only for the Lord Shimura class, the player class has overridden this to include all directions
 */
void ACombatCharacter::Dodge_Implementation()
{
	if (CurrentState != EActionState::None || bParried) return;
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Back"), DodgeMontage);
	CurrentState = EActionState::Dodge;
}

/*
 *	this function is bound to delegate fired by the katana class when it has made contact with the opponent.
 *	this function get the current animation being played and applies the appropriate damage to the opponent
 */
void ACombatCharacter::OnWeaponHitOpponent(FVector ImpactPoint)
{
	FAnimationInfo AnimInfo = AShimuraDuelGameMode::GetInstance()->GetAnimationInfo(GetCurrentAttackAnimation());
	if (!GetOpponent()->IsDodging() && (!GetOpponent()->IsBlocking() || !AnimInfo.bIsDefendable))
	{
		//UE_LOG(LogTemp, Display, TEXT("GetHit"));
		GetOpponent()->GetHit(AnimInfo.Damage, ImpactPoint);
		Cast<AKatana>(Katana->GetChildActor())->DisableCollision();
		if (bInImpactWindow)
		{
			PlayCameraShake();
		}
	}
	else if (GetOpponent()->IsBlocking())
	{
		GetOpponent()->GetStaggerHit(AnimInfo.StaggerDamage);
	}
	else
	{
		Cast<AKatana>(Katana->GetChildActor())->bIsOverlapping = true;
	}
}

/*
 *	this function is bound to the OnDead delegate on the attribute component. This declares the combat character to be dead
 */
void ACombatCharacter::OnDead_Implementation()
{
	CurrentState = EActionState::Dead;
	GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
}

/*
 *	this helper function plays a random section of a given montage
 */
void ACombatCharacter::PlayRandomMontageSection(UAnimMontage* Montage)
{
	if (!Montage) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	int32 RandIndex = FMath::RandRange(0, Montage->GetNumSections()- 1);
	FName RandomSection = Montage->GetSectionName(RandIndex);
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(RandomSection);
}

void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	//get the katana actor from the child actor component
	Katana->GetChildActor()->SetOwner(this);
	Cast<AKatana>(Katana->GetChildActor())->OnHitOpponent.AddDynamic(this, &ThisClass::OnWeaponHitOpponent);

	CurrentStance = EStance::Stone;

	AttributeComponent->OnDead.AddDynamic(this, &ACombatCharacter::OnDead);
}

/*
*	this function does the attack task. It plays a random attack animtion from the appropriate stance montage
 */
void ACombatCharacter::Attack_Implementation()
{
	if (CurrentState != EActionState::None || bParried) return;

	CurrentState = EActionState::Attack;
	PlayRandomMontageSection(StanceAttacks[CurrentStance]);
}

/*
 *	this function disables the collision for the sword when the swing is over. It is called on the animation blueprint animnotify
 */
void ACombatCharacter::FinishSwing_Implementation()
{
	Cast<AKatana>(Katana->GetChildActor())->DisableCollision();
}

/*
 *this functione exits the attack state. if this function has been called, then the attack animation has been successfully completed.
 */
void ACombatCharacter::FinishAttacking_Implementation()
{
	CurrentState = EActionState::None;
	OnAttackFinished.Broadcast(Success);
	bInImpactWindow = false;
}

/*
 *	this function is called at the start of the sword swing. This also declares that the time for parrying is over. If the opponent makes contact with the
 *	sword now, the OnWeaponHitOpponent function gets called
 */
void ACombatCharacter::StartSwing_Implementation()
{
	OutsideParryWindow();
	Cast<AKatana>(Katana->GetChildActor())->EnableCollision();
}

/*
 *	this function does the parry task. It plays the animation and check whether the opponent is close enough and whether
 *	his attack is in the parry window. If so then, the attack is parried.
 */
void ACombatCharacter::Parry_Implementation()
{
	if (CurrentState != EActionState::None && CurrentState != EActionState::Block) return;
	
	CurrentState = EActionState::Parry;

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOpponent()->GetActorLocation(), GetActorLocation());
	float Dist = (GetOpponent()->GetActorLocation() - GetActorLocation()).Size();
	if (GetOpponent()->bInParryWindow && Dist <= 250.f && FMath::IsNearlyEqual(LookAtRotation.Yaw, GetOpponent()->GetActorRotation().Yaw, 10.f))
	{
		GetOpponent()->GetParried();
		bPlayParryEffect = true;
	}
	GetMesh()->GetAnimInstance()->Montage_Play(ParryMontage);
}

/*
 *	this function exits the parry task. It also reset the time dilation incase this parry had been successful
 */
void ACombatCharacter::FinishParry_Implementation()
{
	CurrentState = EActionState::None;
	bPlayParryEffect = false;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}

/*
 *	this function get called when the character has been dealt damage
 */
void ACombatCharacter::GetHit_Implementation(float Damage, FVector ImpactPoint)
{
	if (IsDead()) return;
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;
	AttributeComponent->AddOrRemoveHealth(-Damage);
}

/*
 *	if a character's attack has been parried, then this function is called. Once parried,the character will not be able to block or dodge for 3 seconds, leaving him open
 *	to attack. He can still parry the attack if he can.
 */
void ACombatCharacter::GetParried_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;
	bParried = true;
	GetWorldTimerManager().SetTimer(ParriedTimer, this, &ThisClass::RecoverFromParry, 3.f, false);
}

/*
 *	this function is called at the end of the gethit anim montage
 */
void ACombatCharacter::FinishGetHit_Implementation()
{
	if (!IsDead())
	{
		CurrentState = EActionState::None;
	}
}

/*
 *	this function is fired by an animnotify at the frame which the attack animation has entered the parry window.
 */
void ACombatCharacter::InParryWindow_Implementation()
{
	bInParryWindow = true;
	OnBecomeParriable.Broadcast();
}

/*
 *	this function is fired by an animnotify at the fram which the attack animation has exited the parry window
 */
void ACombatCharacter::OutsideParryWindow_Implementation()
{
	bInParryWindow = false;
}

/*
 *	this function exits the dodge task
 */
void ACombatCharacter::FinishDodging_Implementation()
{
	CurrentState = EActionState::None;
}