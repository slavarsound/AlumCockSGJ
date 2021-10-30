// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResetBlackboardValue.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UBTTask_ResetBlackboardValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ResetBlackboardValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector KeyToReset;
};
