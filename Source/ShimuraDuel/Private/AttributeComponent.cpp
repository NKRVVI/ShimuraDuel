// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeComponent.h"


// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributeComponent::SetPosture(float NewPosture)
{
	CurrentPosture = FMath::Clamp(NewPosture, 0.F, MaxPosture);
	OnPostureChanged.Broadcast(CurrentPosture / MaxPosture);
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
	OnHealthChanged.Broadcast(NewHealth / MaxHealth);
	if (CurrentHealth == 0.f)
	{
		OnDead.Broadcast();
	}
}

