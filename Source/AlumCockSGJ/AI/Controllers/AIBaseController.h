#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIBaseController.generated.h"

class UAISense;
UCLASS()
class ALUMCOCKSGJ_API AAIBaseController : public AAIController
{
	GENERATED_BODY()

public:
	AAIBaseController();

protected:
	AActor* GetClosestSensedActor(TSubclassOf<UAISense> SenseType) const;
};
