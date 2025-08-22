// Fill out your copyright notice in the Description page of Project Settings.


#include "Katana.h"

#include "Characters/CombatCharacter.h"
#include "Components/BoxComponent.h"


// Sets default values
AKatana::AKatana()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AKatana::EnableCollision_Implementation()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AKatana::DisableCollision_Implementation()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKatana::OnHit_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ACombatCharacter>() && OtherActor != Owner)
	{
		ACombatCharacter* Other = Cast<ACombatCharacter>(OtherActor);
		if (!Other->IsDodging() && !Other->IsBlocking())
		{
			Cast<ACombatCharacter>(OtherActor)->GetHit();
			DisableCollision();	
		}
	}
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();

	BoxComp = GetComponentByClass<UBoxComponent>();
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnHit);
}

// Called every frame
void AKatana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

