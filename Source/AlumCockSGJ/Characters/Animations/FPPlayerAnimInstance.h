// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "FPPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UFPPlayerAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayerCameraPitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator CameraBoneRotation;
	
private:
	TWeakObjectPtr<class AFPPlayerCharacter> FPPlayer;
	TWeakObjectPtr<class ABasePlayerController> GCPlayerController;
	float GetCameraPitchAngle() const;
};
