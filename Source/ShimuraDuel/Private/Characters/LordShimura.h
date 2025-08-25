// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CombatCharacter.h"
#include "LordShimura.generated.h"

class AShimuraPlayer;

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

	UPROPERTY(BlueprintReadWrite)
	AShimuraPlayer* Opponent;

	UPROPERTY(BlueprintReadWrite)
	bool bRunning = false;

	virtual void FinishAttacking_Implementation() override;
	virtual void GetHit_Implementation(float Damage) override;
	virtual void GetParried_Implementation() override;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> AttackIntervals = {};

	float NextAttackTime = 0.f;
	float GetHitTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Parry")
	bool bReadyForParry = false;
	
	virtual void Parry_Implementation() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpponentAttackFinished(EAttackEndType CurrentAttackEnd);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnOpponentAttackStarted(EActionState ActionState);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual ACombatCharacter* GetOpponent() const override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure)
	FVector GetLocationInRadius(float Rad);

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EAttackEndType> AttackEnd;
};