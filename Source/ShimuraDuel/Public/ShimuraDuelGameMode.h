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

	UPROPERTY(BlueprintReadWrite)
	TMap<UAnimationAsset*, FAnimationInfo> AnimationInfoMap;

public:
	UFUNCTION(BlueprintPure)
	FORCEINLINE FAnimationInfo GetAnimationInfo(UAnimationAsset* AnimationAsset) const {return AnimationInfoMap.Contains(AnimationAsset) ? AnimationInfoMap[AnimationAsset] : FAnimationInfo();}
};



