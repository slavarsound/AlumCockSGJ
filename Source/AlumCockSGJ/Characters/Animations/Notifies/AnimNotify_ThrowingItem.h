// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/Animations/Notifies/BaseAnimNotify.h"
#include "AnimNotify_ThrowingItem.generated.h"

UENUM(BlueprintType)
enum class EThrowNotifyType : uint8
{
	None = 0,
	Grab = 1,
	Activate = 2,
	Release = 3
};

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_ThrowingItem : public UBaseAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EThrowNotifyType ThrowNotifyType = EThrowNotifyType::None;
};
