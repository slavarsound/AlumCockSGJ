#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "AICharacter.generated.h"

class UAIPatrolComponent;
class UBehaviorTree;
class UAICharacterMovementComponent;

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAICharacter(const FObjectInitializer& ObjectInitializer);

	UAIPatrolComponent* GetAIPatrolComponent() const { return AIPatrolComponent; }
	UBehaviorTree* GetBehaviorTree() const { return DefaultBehaviorTree; }

	class UAICharacterMovementComponent* GetAIMovementComponent() const { return AIMovementComponent.Get(); }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPatrolComponent* AIPatrolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* DefaultBehaviorTree;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// class UNavigationInvokerComponent* NavigationInvokerComponent;

private:
	TWeakObjectPtr<UAICharacterMovementComponent> AIMovementComponent;

};
