#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:
	const TArray<FVector>& GetWaypoints() const { return Waypoints; }
	void Reverse();
	
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta=(MakeEditWidget))
	TArray<FVector> Waypoints;
};
