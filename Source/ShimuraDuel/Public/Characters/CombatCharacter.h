// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatCharacter.generated.h"

UENUM(Blueprintable)
enum class EActionState : uint8
{
	None UMETA(DisplayName = "None"),
	Attack UMETA(DisplayName = "Attack"),
	Dodge UMETA(DisplayName = "Dodge"),
	Block UMETA(DisplayName = "Block"),
	Parry UMETA(DisplayName = "Parry"),
	GetHit UMETA(DisplayName = "GetHit")
};

UCLASS()
class SHIMURADUEL_API ACombatCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatCharacter();

	UPROPERTY(EditAnywhere)
	UChildActorComponent* Katana;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetHit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	EActionState CurrentState = EActionState::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CombatCharacter")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CombatCharacter")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CombatCharacter")
	UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CombatCharacter")
	UAnimMontage* GetHitMontage;

	UPROPERTY(BlueprintReadWrite)
	bool bInMiddleOfSwing = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsParriable = false;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishSwing();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishAttacking();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartAttacking();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Parry();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishParry();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishGetHit();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDodging() const { return CurrentState == EActionState::Dodge; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsBlocking() const { return CurrentState == EActionState::Block; }
};