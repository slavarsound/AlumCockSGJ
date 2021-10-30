// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "GCGameMode.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AGCGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void StartPlay() override;
};
