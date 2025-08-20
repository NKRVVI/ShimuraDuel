// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Stances.h"
#include "GameFramework/Character.h"
#include "Shimura.generated.h"

UCLASS()
class SHIMURADUEL_API AShimura : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AShimura();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite)
	EStance CurrentStance;
};
