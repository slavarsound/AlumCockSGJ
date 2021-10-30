#include "NavLinkProxySlide.h"

#include "NavLinkCustomComponent.h"
#include "Characters/BaseCharacter.h"

ANavLinkProxySlide::ANavLinkProxySlide()
{
	bSmartLinkIsRelevant = true;
}

void ANavLinkProxySlide::BeginPlay()
{
	Super::BeginPlay();
	OnSmartLinkReached.AddDynamic(this, &ANavLinkProxySlide::InvokeSlide);
}

void ANavLinkProxySlide::InvokeSlide(AActor* Actor, const FVector& DestinationPoint)
{
	ABaseCharacter* Character = Cast<ABaseCharacter>(Actor);
	if (IsValid(Character))
	{
		Character->TryStartSliding();
	}
}

void ANavLinkProxySlide::OnSlideEnded()
{
}