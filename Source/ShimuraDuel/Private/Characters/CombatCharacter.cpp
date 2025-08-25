// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CombatCharacter.h"

#include "AttributeComponent.h"
#include "Katana.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ACombatCharacter::ACombatCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Katana = CreateDefaultSubobject<UChildActorComponent>("Katana");
	Katana->SetupAttachment(GetMesh(), FName("katana_r"));

	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>("Attributes");
}

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

void ACombatCharacter::Dodge_Implementation()
{
	if (CurrentState != EActionState::None) return;
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Back"), DodgeMontage);
	CurrentState = EActionState::Dodge;
}

void ACombatCharacter::OnWeaponHitOpponent()
{
	if (!GetOpponent()->IsDodging() && !GetOpponent()->IsBlocking())
	{
		//UE_LOG(LogTemp, Display, TEXT("GetHit"));
		GetOpponent()->GetHit();
		Cast<AKatana>(Katana->GetChildActor())->DisableCollision();
	}
	else
	{
		Cast<AKatana>(Katana->GetChildActor())->bIsOverlapping = true;
	}
}

void ACombatCharacter::PlayRandomMontageSection(UAnimMontage* Montage)
{
	if (!Montage) return;
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	int32 RandIndex = FMath::RandRange(0, Montage->GetNumSections()- 1);
	FName RandomSection = Montage->GetSectionName(RandIndex);
	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(RandomSection);
}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	Katana->GetChildActor()->SetOwner(this);
	Cast<AKatana>(Katana->GetChildActor())->OnHitOpponent.AddDynamic(this, &ThisClass::OnWeaponHitOpponent);

	CurrentStance = EStance::Stone;
}

void ACombatCharacter::Attack_Implementation()
{
	if (CurrentState != EActionState::None) return;

	CurrentState = EActionState::Attack;
	PlayRandomMontageSection(StanceAttacks[CurrentStance]);
}

void ACombatCharacter::FinishSwing_Implementation()
{
	Cast<AKatana>(Katana->GetChildActor())->DisableCollision();
}

void ACombatCharacter::FinishAttacking_Implementation()
{
	CurrentState = EActionState::None;
	OnAttackFinished.Broadcast(Success);
}

//POST TELEGRAPH
void ACombatCharacter::StartSwing_Implementation()
{
	OutsideParryWindow();
	Cast<AKatana>(Katana->GetChildActor())->EnableCollision();
}

void ACombatCharacter::Parry_Implementation()
{
	if (CurrentState != EActionState::None && CurrentState != EActionState::Block) return;
	
	CurrentState = EActionState::Parry;

	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetOpponent()->GetActorLocation(), GetActorLocation());
	float Dist = (GetOpponent()->GetActorLocation() - GetActorLocation()).Size();
	if (GetOpponent()->bInParryWindow && Dist <= 250.f && FMath::IsNearlyEqual(LookAtRotation.Yaw, GetOpponent()->GetActorRotation().Yaw, 10.f))
	{
		GetOpponent()->GetParried();
	}
	GetMesh()->GetAnimInstance()->Montage_Play(ParryMontage);
}

void ACombatCharacter::FinishParry_Implementation()
{
	CurrentState = EActionState::None;
}

void ACombatCharacter::GetHit_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;	
}

void ACombatCharacter::GetParried_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;
}

void ACombatCharacter::FinishGetHit_Implementation()
{
	CurrentState = EActionState::None;
}

void ACombatCharacter::InParryWindow_Implementation()
{
	bInParryWindow = true;
	OnBecomeParriable.Broadcast();
}

void ACombatCharacter::OutsideParryWindow_Implementation()
{
	bInParryWindow = false;
}

void ACombatCharacter::FinishDodging_Implementation()
{
	CurrentState = EActionState::None;
}

// Called every frame
void ACombatCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACombatCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

