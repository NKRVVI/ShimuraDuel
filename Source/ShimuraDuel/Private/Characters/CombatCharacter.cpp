// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CombatCharacter.h"


// Sets default values
ACombatCharacter::ACombatCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACombatCharacter::Attack_Implementation()
{
	if (bAttacking) return;
	
	bAttacking = true;
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
}

void ACombatCharacter::FinishAttacking_Implementation()
{
	bAttacking = false;
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

