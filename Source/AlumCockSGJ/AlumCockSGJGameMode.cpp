// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlumCockSGJGameMode.h"
#include "Characters/AlumCockSGJCharacter.h"
#include "UObject/ConstructorHelpers.h"

AAlumCockSGJGameMode::AAlumCockSGJGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
