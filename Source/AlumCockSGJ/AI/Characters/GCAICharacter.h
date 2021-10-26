#pragma once

#include "CoreMinimal.h"
#include "Characters/GCBaseCharacter.h"
#include "GCAICharacter.generated.h"

class UAIPatrolComponent;
class UBehaviorTree;

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AGCAICharacter : public AGCBaseCharacter
{
	GENERATED_BODY()

public:
	AGCAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrolComponent* GetAIPatrolComponent() const { return AIPatrolComponent; }
	UBehaviorTree* GetBehaviorTree() const { return DefaultBehaviorTree; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPatrolComponent* AIPatrolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* DefaultBehaviorTree;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// class UNavigationInvokerComponent* NavigationInvokerComponent;
};
