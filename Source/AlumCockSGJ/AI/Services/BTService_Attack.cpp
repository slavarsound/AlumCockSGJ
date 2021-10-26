#include "AI/Services/BTService_Attack.h"

#include "AIController.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/GCBaseCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"


UBTService_Attack::UBTService_Attack()
{
	NodeName = "Attack";
}

void UBTService_Attack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Controller) || !IsValid(Blackboard)) return;

	AGCBaseCharacter* BotCharacter = Cast<AGCBaseCharacter>(Controller->GetPawn());
	if (!IsValid(BotCharacter)) return;

	const UCharacterEquipmentComponent* EquipmentComponent = BotCharacter->GetEquipmentComponent();
	const ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();
	if (!IsValid(RangeWeapon)) return;

	AActor* CurrentTarget = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(CurrentTarget))
	{
		BotCharacter->StopFiring();
		return;
	}

	float DistSq = FVector::DistSquared(BotCharacter->GetActorLocation(), CurrentTarget->GetActorLocation());
	if (DistSq > FMath::Square(MaxAttackRange))
	{
		BotCharacter->StopFiring();
		return;
	}

	if (!EquipmentComponent->IsReloading() && !RangeWeapon->IsFiring())
	{
		BotCharacter->StartFiring();
	}
}
