#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GCAIController.generated.h"

class UAISense;
UCLASS()
class ALUMCOCKSGJ_API AGCAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGCAIController();

protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseType) const;
};
