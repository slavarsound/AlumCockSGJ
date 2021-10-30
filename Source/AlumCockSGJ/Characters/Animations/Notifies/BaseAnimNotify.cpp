#include "BaseAnimNotify.h"

#include "Characters/BaseCharacter.h"

ABaseCharacter* UBaseAnimNotify::GetCharacter(AActor* Actor) const
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
	if (!IsValid(Character))
	{
		return nullptr;
	}

	return Character;
}
