#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_Attack.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_Attack : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Attack();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAttackRange = 1000.f;
};
