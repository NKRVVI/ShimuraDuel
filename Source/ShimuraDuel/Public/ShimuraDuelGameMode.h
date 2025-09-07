// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Utils/AnimationInfo.h"
#include "ShimuraDuelGameMode.generated.h"


UCLASS(minimalapi)
class AShimuraDuelGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AShimuraDuelGameMode();

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintPure)
	static AShimuraDuelGameMode* GetInstance();

protected:
	static AShimuraDuelGameMode* Instance;

	UPROPERTY(EditAnywhere)
	UDataTable* AnimationInfoDataTable;

	/*
	 *	this data table contains the information mapping every used attack animation asset to its data, like damage, stagger damage and whether it is defendable or not
	 */
	UPROPERTY(BlueprintReadWrite)
	TMap<UAnimationAsset*, FAnimationInfo> AnimationInfoMap;

public:
	/*
	 *	for a given animation asset, the animationinfo is given
	 */
	UFUNCTION(BlueprintPure)
	FORCEINLINE FAnimationInfo GetAnimationInfo(UAnimationAsset* AnimationAsset) const {return AnimationInfoMap.Contains(AnimationAsset) ? AnimationInfoMap[AnimationAsset] : FAnimationInfo();}
};



