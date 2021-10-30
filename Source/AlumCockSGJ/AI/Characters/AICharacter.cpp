#include "AICharacter.h"

#include "NavigationInvokerComponent.h"
#include "AI/Components/AICharacterMovementComponent.h"
#include "AI/Components/AIPatrolComponent.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AIMovementComponent = StaticCast<UAICharacterMovementComponent*>(GetCharacterMovement());
	
	AIPatrolComponent = CreateDefaultSubobject<UAIPatrolComponent>(TEXT("PatrolComponent"));
	AddOwnedComponent(AIPatrolComponent);

	// NavigationInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));
	// AddOwnedComponent(NavigationInvokerComponent);
}