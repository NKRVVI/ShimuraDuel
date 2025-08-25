// Fill out your copyright notice in the Description page of Project Settings.


#include "Katana.h"

#include "Characters/CombatCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AKatana::AKatana()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KatanaMesh = CreateDefaultSubobject<UStaticMeshComponent>("KatanaMesh");
	SetRootComponent(KatanaMesh);
	
	BoxCompStart = CreateDefaultSubobject<USceneComponent>("BoxCompStart");
	BoxCompStart->SetupAttachment(GetRootComponent());

	BoxCompEnd = CreateDefaultSubobject<USceneComponent>("BoxCompEnd");
	BoxCompEnd->SetupAttachment(GetRootComponent());
}

void AKatana::EnableCollision_Implementation()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AKatana::DisableCollision_Implementation()
{
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKatana::LineTraceHitActor(AActor* OtherActor)
{
	FVector StartPos = BoxCompStart->GetComponentLocation();
	FVector EndPos = BoxCompEnd->GetComponentLocation();

	TArray<AActor*> IgnoredActors = {};
	IgnoredActors.Add(GetOwner());
	IgnoredActors.Add(Cast<ACombatCharacter>(Cast<ACombatCharacter>(OtherActor)->GetOpponent())->Katana->GetChildActor());

	FHitResult Hit;
		
	if (UKismetSystemLibrary::BoxTraceSingle(this, StartPos, EndPos, BoxTraceExtent, BoxCompStart->GetComponentRotation(), UEngineTypes::ConvertToTraceType(ECC_Pawn), true, IgnoredActors, EDrawDebugTrace::None, Hit, true))
	{
		OnHitOpponent.Broadcast(Hit.ImpactPoint);
	}
}

void AKatana::OnHit_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACombatCharacter>(GetOwner())->GetOpponent() == OtherActor)
	{
		LineTraceHitActor(OtherActor);
	}
}

void AKatana::OnHitEnd_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ACombatCharacter>(GetOwner())->GetOpponent() == OtherActor)
	{
		bIsOverlapping = false;
	}
}

// Called when the game starts or when spawned
void AKatana::BeginPlay()
{
	Super::BeginPlay();

	BoxComp = GetComponentByClass<UBoxComponent>();
	BoxComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnHit);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnHitEnd);
}

// Called every frame
void AKatana::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (BoxComp->GetCollisionEnabled() != ECollisionEnabled::NoCollision && bIsOverlapping)
	{
		if (BoxComp->IsOverlappingActor(Cast<ACombatCharacter>(GetOwner())->GetOpponent()))
		{
			LineTraceHitActor(Cast<ACombatCharacter>(GetOwner())->GetOpponent());
			/*ACombatCharacter* Opponent = Cast<ACombatCharacter>(GetOwner())->GetOpponent();
			if (!Opponent->IsDodging() && !Opponent->IsBlocking())
			{
				//UE_LOG(LogTemp, Display, TEXT("GetHitTick"));
				Opponent->GetHit();
				DisableCollision();
			}*/
		}
	}
}

