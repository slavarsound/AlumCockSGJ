// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/GCProjectile.h"
#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API AExplosiveProjectile : public AGCProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();

	virtual void Activate(AController* ThrowerController) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DetonationTime = 3.f;
	
	virtual void OnProjectileLaunched() override;

	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:
	FTimerHandle DetonationTimer;

	void Detonate();
};
