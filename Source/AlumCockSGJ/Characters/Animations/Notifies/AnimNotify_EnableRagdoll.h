// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GCAnimNotify.h"
#include "AnimNotify_EnableRagdoll.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAnimNotify_EnableRagdoll : public UGCAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;	
};
