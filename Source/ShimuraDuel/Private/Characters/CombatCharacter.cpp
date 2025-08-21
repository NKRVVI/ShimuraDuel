// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CombatCharacter.h"

#include "Katana.h"


// Sets default values
ACombatCharacter::ACombatCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Katana = CreateDefaultSubobject<UChildActorComponent>("Katana");
	Katana->SetupAttachment(GetMesh(), FName("katana_r"));
}

// Called when the game starts or when spawned
void ACombatCharacter::BeginPlay()
{
	Super::BeginPlay();

	Katana->GetChildActor()->SetOwner(this);
}

void ACombatCharacter::Attack_Implementation()
{
	if (bAttacking) return;
	
	bAttacking = true;
	GetMesh()->GetAnimInstance()->Montage_Play(AttackMontage);
	bInMiddleOfSwing = true;
}

void ACombatCharacter::FinishAttacking_Implementation()
{
	bAttacking = false;
	Cast<AKatana>(Katana->GetChildActor())->DisableCollision();
	bInMiddleOfSwing = false;
}

//POST TELEGRAPH
void ACombatCharacter::StartAttacking_Implementation()
{
	Cast<AKatana>(Katana->GetChildActor())->EnableCollision();
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

