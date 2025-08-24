// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CombatCharacter.h"
#include "LordShimura.generated.h"

UENUM(BlueprintType)
enum EAttackEndType : uint8
{
	Success UMETA(DisplayName = "Success"),
	GetHit UMETA(DisplayName = "GetHit"),
	Parried UMETA(DisplayName = "Parried"),
};

class AShimuraPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackFinished, EAttackEndType, EndType);

UCLASS()
class SHIMURADUEL_API ALordShimura : public ACombatCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALordShimura();

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackFinished OnAttackFinished;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpponentDodgeFinished();

	UPROPERTY(BlueprintReadWrite)
	AShimuraPlayer* Opponent;

	UPROPERTY(BlueprintReadWrite)
	bool bRunning = false;

	virtual void FinishAttacking_Implementation() override;
	virtual void GetHit_Implementation() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual ACombatCharacter* GetOpponent() const override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	FVector GetLocationInRadius(float Rad);
};
