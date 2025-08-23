#pragma once

#include "Stances.generated.h"

UENUM(BlueprintType)
enum class EStance : uint8
{
	Stone UMETA(DisplayName = "Stone"),
	Water UMETA(DisplayName = "Water"),
	Wind UMETA(DisplayName = "Wind"),
	Moon UMETA(DisplayName = "Moon")
};