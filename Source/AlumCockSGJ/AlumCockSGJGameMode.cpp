#include "AlumCockSGJGameMode.h"
#include "Characters/AlumCockSGJCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Controllers/AlumCockSGJPlayerController.h"

AAlumCockSGJGameMode::AAlumCockSGJGameMode()
{
	DefaultPawnClass = AAlumCockSGJCharacter::StaticClass();
	PlayerControllerClass = AAlumCockSGJPlayerController::StaticClass();
}
