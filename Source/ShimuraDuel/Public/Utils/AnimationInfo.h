#pragma once

#include "CoreMinimal.h"
#include "AnimationInfo.generated.h"


USTRUCT(BlueprintType)
struct FAnimationInfo : public FTableRowBase
{
GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UAnimationAsset* AnimationAsset = nullptr;

	UPROPERTY(EditAnywhere)
	bool bIsDefendable = false;

	UPROPERTY(EditAnywhere)
	float StaggerDamage = 0.f;

	UPROPERTY(EditAnywhere)
	float Damage = 0.f;
};