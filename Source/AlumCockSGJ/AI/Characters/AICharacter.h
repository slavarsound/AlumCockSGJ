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

	virtual void PossessedBy(AController* NewController) override;
	ETeam GetTeam() const { return Team; }
	
	class UAICharacterMovementComponent* GetAIMovementComponent() const { return AIMovementComponent.Get(); }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPatrolComponent* AIPatrolComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBehaviorTree* DefaultBehaviorTree;
	
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// class UNavigationInvokerComponent* NavigationInvokerComponent;

	virtual void ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
		AActor* DamageCauser) override;
	
private:
	TWeakObjectPtr<UAICharacterMovementComponent> AIMovementComponent;

};
