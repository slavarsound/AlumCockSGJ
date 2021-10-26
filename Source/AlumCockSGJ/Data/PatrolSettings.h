#pragma once

#include "PatrolSettings.generated.h"

UENUM(BlueprintType)
enum class EPatrolType : uint8
{
	Circle,
	PingPong
};

USTRUCT(BlueprintType)
struct FPatrolSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class APatrolPath* PatrolPath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPatrolType PatrolType = EPatrolType::Circle;
};