// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DebugSubsystem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UDebugSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	bool IsDebugCategoryEnabled(const FName& CategoryName) const;

private:
	UFUNCTION(Exec)
	void SetDebugCategoryEnabled(const FName& CategoryName, bool bEnabled);

	TMap<FName, bool> CategoriesStates;
};
