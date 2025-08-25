// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Katana.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHitOpponent);

UCLASS()
class SHIMURADUEL_API AKatana : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKatana();

	UBoxComponent* BoxComp;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnableCollision();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void DisableCollision();

	UPROPERTY(BlueprintReadOnly)
	bool bIsOverlapping = false;

	UFUNCTION(BlueprintNativeEvent)
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintNativeEvent)
	void OnHitEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);


	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FHitOpponent OnHitOpponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
