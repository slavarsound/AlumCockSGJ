#include "AICharacter.h"

#include "NavigationInvokerComponent.h"
#include "AI/Components/AICharacterMovementComponent.h"
#include "AI/Components/AIPatrolComponent.h"
#include "Perception/AISense_Damage.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UAICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	AIMovementComponent = StaticCast<UAICharacterMovementComponent*>(GetCharacterMovement());
	
	AIPatrolComponent = CreateDefaultSubobject<UAIPatrolComponent>(TEXT("PatrolComponent"));
	AddOwnedComponent(AIPatrolComponent);

	// NavigationInvokerComponent = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("NavigationInvoker"));
	// AddOwnedComponent(NavigationInvokerComponent);
}

void AAICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	IGenericTeamAgentInterface* AIController = Cast<IGenericTeamAgentInterface>(NewController);
	if (!AIController)
	{
		return;
	}

	AIController->SetGenericTeamId(FGenericTeamId((uint8)Team));
}

void AAICharacter::ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* Damage1,
                                 AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReactToDamage(DamagedActor, Damage, Damage1, InstigatedBy, DamageCauser);
	if (!CharacterAttributesComponent->IsAlive())
		return;

	UAISense_Damage::ReportDamageEvent(GetWorld(), this, DamageCauser, Damage,
		DamageCauser->GetActorLocation(), GetActorLocation());
}
