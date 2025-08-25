// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"


// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::SetPosture(float NewPosture)
{
	CurrentPosture = FMath::Clamp(NewPosture, 0.F, MaxPosture);
}

void UAttributeComponent::AddOrRemovePosture_Implementation(float Increment)
{
	SetPosture(CurrentPosture + Increment);
}

void UAttributeComponent::AddOrRemoveHealth_Implementation(float Increment)
{
	SetHealth(CurrentHealth + Increment);
}

void UAttributeComponent::SetHealth(float NewHealth)
{
	CurrentHealth = FMath::Clamp(NewHealth, 0, MaxHealth);
}

