// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GCBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UGCBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	float InputForward = 0;
	
	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	float InputRight = 0;

	UPROPERTY(BlueprintReadOnly, Transient, Category="Base pawn animation instance")
	bool bInAir = false;

private:
	TWeakObjectPtr<class AGCBasePawn> Pawn;
};
