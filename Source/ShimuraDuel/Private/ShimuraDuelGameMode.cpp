// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShimuraDuel/Public/ShimuraDuelGameMode.h"
#include "UObject/ConstructorHelpers.h"

AShimuraDuelGameMode* AShimuraDuelGameMode::Instance = nullptr;

AShimuraDuelGameMode::AShimuraDuelGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	Instance = this;
}
/*
 *	we store the datatables info in a hashmap, between the animation asset and the animationinfo struct
 */
void AShimuraDuelGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<FAnimationInfo*> AnimationInfos;
	AnimationInfoDataTable->GetAllRows<FAnimationInfo>(TEXT(""), AnimationInfos);

	for (auto AnimationInfo : AnimationInfos)
	{
		AnimationInfoMap.Add(AnimationInfo->AnimationAsset, *AnimationInfo);
	}
}

/*
 *	static helper function for easy retrieval of the gamemode
 */
AShimuraDuelGameMode* AShimuraDuelGameMode::GetInstance()
{
	return Instance;
}
