// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CombatCharacter.h"
#include "LordShimura.generated.h"

UCLASS()
class SHIMURADUEL_API ALordShimura : public ACombatCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALordShimura();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
