#include "AICharacterController.h"

#include "GameCode.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Components/AIPatrolComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"

void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void AAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AAICharacter>(), TEXT("AAICharacterController is intended to be used only with AGCAICharacter pawn"))
		ControlledCharacter = StaticCast<AAICharacter*>(InPawn);
		RunBehaviorTree(ControlledCharacter->GetBehaviorTree());
		if (ControlledCharacter->GetAIPatrolComponent()->CanPatrol())
		{
			Blackboard->SetValueAsObject(BB_FocusActor, ControlledCharacter->GetAIPatrolComponent()->GetFocusActor());
			Blackboard->SetValueAsVector(BB_FocusPoint, ControlledCharacter->GetAIPatrolComponent()->GetFocusPoint());
		}
	}
}

void AAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);

	for (const auto PerceptedActor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo ActorPerceptionInfo;
		bool bHasInfo = PerceptionComponent->GetActorsPerception(PerceptedActor, ActorPerceptionInfo);
		if (!bHasInfo)
			continue;
	}
}

void AAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
}

void AAICharacterController::TryMoveToNextTarget()
{
	AActor* ActorToFollow = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ActorToFollow) && IsValid(Blackboard) && ControlledCharacter->GetAIPatrolComponent()->IsAggressive())
	{
		Blackboard->SetValueAsObject(BB_CurrentTarget, ActorToFollow);
		SetFocus(ActorToFollow, EAIFocusPriority::Gameplay);
		bPatrolling = false;
	}
	else
	{
		Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		ClearFocus(EAIFocusPriority::Gameplay);
		TryPatrol();
	}	
}

void AAICharacterController::TryPatrol()
{
	UAIPatrolComponent* PatrolComponent = ControlledCharacter->GetAIPatrolComponent();
	if (PatrolComponent->CanPatrol() && IsValid(Blackboard))
	{
		FVector NextWaypoint;
		if (bPatrolling)
		{
			NextWaypoint = PatrolComponent->SelectNextWaypoint();
		}
		else
		{
			NextWaypoint = PatrolComponent->SelectClosestWaypoint();
			bPatrolling = true;
		}

		Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		Blackboard->SetValueAsVector(BB_NextLocation, NextWaypoint);
	}
}

bool AAICharacterController::IsTargetReached(FVector TargetLocation, float TargetReachRadius) const
{
	return (TargetLocation - ControlledCharacter->GetActorLocation()).SizeSquared() <= (TargetReachRadius * TargetReachRadius);
}