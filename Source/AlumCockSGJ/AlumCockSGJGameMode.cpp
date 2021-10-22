#include "AlumCockSGJGameMode.h"
#include "Characters/AlumCockSGJCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAlumCockSGJGameMode::AAlumCockSGJGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
