// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterAttributesWidget.h"
#include "Blueprint/UserWidget.h"
#include "Data/CharacterTypes.h"
#include "Data/UserInterfaceTypes.h"
#include "PlayerHUDWidget.generated.h"

UCLASS(BlueprintType)
class ALUMCOCKSGJ_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void OnAimingStateChanged(bool bAiming, EReticleType ReticleType);
	void SetAmmo(int32 ClipAmmo, int32 RemainingAmmo);
	void SetThrowablesCount(int32 Count);
	void OnAttributeChanged(ECharacterAttribute Attribute, float Value);
	void SetWeaponName(const FText& Name);
	void SetThrowableName(const FText& Name);

	void SetReticleType(EReticleType ReticleType);
	void OnMeleeWeaponEquipped();

protected:

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UReticleWidget* Reticle;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UWeaponInfoWidget* WeaponInfoWidget;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UCharacterAttributesWidget* CharacterAttributesWidget;
};
