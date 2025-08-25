// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHIMURADUEL_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAttributeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CurrentHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float CurrentPosture = 100.f;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float MaxPosture = 100.f;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable)
	void SetPosture(float NewPosture);

	UFUNCTION(BlueprintNativeEvent)
	void AddOrRemoveHealth(float Increment);

	UFUNCTION(BlueprintNativeEvent)
	void AddOrRemovePosture(float Increment);

	UFUNCTION(BlueprintPure)
	float GetHealth() {return CurrentHealth;}
	UFUNCTION(BlueprintPure)
	float GetPosture() {return CurrentPosture;}
	
};
