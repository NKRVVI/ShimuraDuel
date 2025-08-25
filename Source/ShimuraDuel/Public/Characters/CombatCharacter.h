// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Utils/Stances.h"
#include "CombatCharacter.generated.h"

class UAttributeComponent;

UENUM(BlueprintType)
enum class EActionState : uint8
{
	None UMETA(DisplayName = "None"),
	Attack UMETA(DisplayName = "Attack"),
	Dodge UMETA(DisplayName = "Dodge"),
	Block UMETA(DisplayName = "Block"),
	Parry UMETA(DisplayName = "Parry"),
	GetHit UMETA(DisplayName = "GetHit")
};

UENUM(BlueprintType)
enum EAttackEndType : uint8
{
	Success UMETA(DisplayName = "Success"),
	GetHit UMETA(DisplayName = "GetHit"),
	Parried UMETA(DisplayName = "Parried"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttackFinished, EAttackEndType, EndType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBecomeParriable);

UCLASS()
class SHIMURADUEL_API ACombatCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACombatCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UChildActorComponent* Katana;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetHit();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void GetParried();

	UPROPERTY(BlueprintReadWrite)
	bool bInParryWindow = false;

	UFUNCTION(BlueprintPure)
	UAnimationAsset* GetCurrentAttackAnimation();

	UPROPERTY(BlueprintReadWrite)
	EStance CurrentStance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EStance, UAnimMontage*> StanceAttacks;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAttackFinished OnAttackFinished;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FBecomeParriable OnBecomeParriable;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UAttributeComponent* AttributeComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
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

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Attack();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishSwing();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishAttacking();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void StartSwing();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Parry();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishParry();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishGetHit();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InParryWindow();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OutsideParryWindow();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void FinishDodging();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Dodge();

	UFUNCTION(BlueprintCallable)
	void PlayRandomMontageSection(UAnimMontage* Montage);

	UFUNCTION()
	void OnWeaponHitOpponent();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsDodging() const { return CurrentState == EActionState::Dodge; }
	UFUNCTION(BlueprintPure)
	FORCEINLINE bool IsBlocking() const { return CurrentState == EActionState::Block; }
	UFUNCTION(BlueprintPure, BlueprintCallable)
	virtual ACombatCharacter* GetOpponent() const {return nullptr;}
};