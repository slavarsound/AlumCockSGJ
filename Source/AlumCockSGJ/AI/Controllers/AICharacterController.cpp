#include "AICharacterController.h"

#include "GameCode.h"
#include "AI/Characters/GCAICharacter.h"
#include "AI/Components/AIPatrolComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"

void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	if (!ControlledCharacter.IsValid())
	{
		return;
	}

	TryPatrol();
}

void AAICharacterController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	if (IsValid(InPawn))
	{
		checkf(InPawn->IsA<AGCAICharacter>(), TEXT("AAICharacterController is intended to be used only with AGCAICharacter pawn"))
		ControlledCharacter = StaticCast<AGCAICharacter*>(InPawn);
		RunBehaviorTree(ControlledCharacter->GetBehaviorTree());
	}
}

void AAICharacterController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	if (!ControlledCharacter.IsValid())
	{
		return;
	}

	TryMoveToNextTarget();
}

void AAICharacterController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	if (!Result.IsSuccess())
	{
		return;
	}

	if (bPatrolling)
	{
		TryMoveToNextTarget();
	}
	else
	{
		// preventing stack overflow by adding some delay before starting chasing actor again
		GetWorld()->GetTimerManager().SetTimer(ActorFollowCooldownTimer, this, &AAICharacterController::TryMoveToNextTarget, ActorFollowCooldown);
	}
}

void AAICharacterController::TryMoveToNextTarget()
{
	AActor* ActorToFollow = GetClosestSensedActor(UAISense_Sight::StaticClass());
	if (IsValid(ActorToFollow) && IsValid(Blackboard))
	{
		Blackboard->SetValueAsObject(BB_CurrentTarget, ActorToFollow);
		SetFocus(ActorToFollow, EAIFocusPriority::Gameplay);
		bPatrolling = false;
	}
	else
	{
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
	// FVector ControlledPawnLocation =  ControlledCharacter->GetActorLocation();
	// float DistanceSquared = (TargetLocation - ControlledPawnLocation).SizeSquared();
	// float TargetReachSquareRad = TargetReachRadius * TargetReachRadius;
	// return DistanceSquared < TargetReachSquareRad;
	return (TargetLocation - ControlledCharacter->GetActorLocation()).SizeSquared() <= (TargetReachRadius * TargetReachRadius);
}

inline void AAICharacterController::OnFollowCooldownElapsed()
{
	bCanFollowActor = true;
	TryMoveToNextTarget();
}