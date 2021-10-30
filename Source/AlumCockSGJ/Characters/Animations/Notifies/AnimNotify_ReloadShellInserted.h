// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAnimNotify.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ReloadShellInserted.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_ReloadShellInserted : public UBaseAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, UIMin=1))
	uint8 ShellsInsertedAtOnce = 1;
	
};
