#pragma once

#include "CoreMinimal.h"
#include "AnimationInfo.generated.h"

/*
 *	every attack animation used has a mapped animation info with all its data
 */
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