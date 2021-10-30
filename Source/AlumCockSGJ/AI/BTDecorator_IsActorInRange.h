#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsActorInRange.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTDecorator_IsActorInRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinRange = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxRange = 1500.f;
};
