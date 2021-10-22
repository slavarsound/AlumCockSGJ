#pragma once

#include "CoreMinimal.h"
#include "AlumCockSGJPlayerController.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AAlumCockSGJPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAlumCockSGJPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
