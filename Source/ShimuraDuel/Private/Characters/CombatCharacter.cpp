// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CombatCharacter.h"

#include "AttributeComponent.h"
#include "Katana.h"
#include "ShimuraDuelGameMode.h"
#include "Kismet/GameplayStatics.h"
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

void ACombatCharacter::PlayParryEffect_Implementation()
{
	if (!bPlayParryEffect) return;
	
	if (AKatana* Sword = Cast<AKatana>(Katana->GetChildActor()))
	{
		Sword->PlayParryEffect();
	}
	UGameplayStatics::SetGlobalTimeDilation(this, 0.25f);
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
	if (CurrentState != EActionState::None || bParried) return;
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(FName("Back"), DodgeMontage);
	CurrentState = EActionState::Dodge;
}

void ACombatCharacter::OnWeaponHitOpponent(FVector ImpactPoint)
{
	FAnimationInfo AnimInfo = AShimuraDuelGameMode::GetInstance()->GetAnimationInfo(GetCurrentAttackAnimation());
	check(AnimInfo.Damage > 0.f);
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

void ACombatCharacter::OnDead_Implementation()
{
	CurrentState = EActionState::Dead;
	GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
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

	AttributeComponent->OnDead.AddDynamic(this, &ACombatCharacter::OnDead);
}

void ACombatCharacter::Attack_Implementation()
{
	if (CurrentState != EActionState::None || bParried) return;

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
	bInImpactWindow = false;
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
		bPlayParryEffect = true;
	}
	GetMesh()->GetAnimInstance()->Montage_Play(ParryMontage);
}

void ACombatCharacter::FinishParry_Implementation()
{
	CurrentState = EActionState::None;
	bPlayParryEffect = false;
	UGameplayStatics::SetGlobalTimeDilation(this, 1.f);
}

void ACombatCharacter::GetHit_Implementation(float Damage, FVector ImpactPoint)
{
	if (IsDead()) return;
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;
	AttributeComponent->AddOrRemoveHealth(-Damage);
}

void ACombatCharacter::GetParried_Implementation()
{
	GetMesh()->GetAnimInstance()->Montage_Play(GetHitMontage);
	CurrentState = EActionState::GetHit;
	bParried = true;
	GetWorldTimerManager().SetTimer(ParriedTimer, this, &ThisClass::RecoverFromParry, 3.f, false);
}

void ACombatCharacter::FinishGetHit_Implementation()
{
	if (!IsDead())
	{
		CurrentState = EActionState::None;
	}
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

