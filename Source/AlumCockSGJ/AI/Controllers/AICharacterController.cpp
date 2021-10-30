#include "AICharacterController.h"

#include "GameCode.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Components/AIPatrolComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

void AAICharacterController::BeginPlay()
{
	Super::BeginPlay();
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAICharacterController::OnTargetPerceptionUpdated);
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

	// for (const auto PerceptedActor : UpdatedActors)
	// {
	// 	FActorPerceptionBlueprintInfo ActorPerceptionInfo;
	// 	bool bHasInfo = PerceptionComponent->GetActorsPerception(PerceptedActor, ActorPerceptionInfo);
	// 	if (!bHasInfo)
	// 		continue;
	//
	// 	for(const auto& Stimulus: ActorPerceptionInfo.LastSensedStimuli)
	// 	{
	// 		TSubclassOf<UAISense> StimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
	// 		if (StimulusClass == UAISense_Sight::StaticClass())
	// 		{
	// 			if (Stimulus.IsActive())
	// 			{
	// 				
	// 			}
	// 			// Blackboard->SetValueAsObject()
	// 		}
	// 	}
	// }
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
		Blackboard->SetValueAsObject(BB_CurrentVisualTarget, ActorToFollow);
		SetFocus(ActorToFollow, EAIFocusPriority::Gameplay);
		bPatrolling = false;
	}
	else
	{
		Blackboard->SetValueAsObject(BB_CurrentVisualTarget, nullptr);
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

		Blackboard->SetValueAsObject(BB_CurrentVisualTarget, nullptr);
		Blackboard->SetValueAsVector(BB_NextLocation, NextWaypoint);
	}
}

bool AAICharacterController::IsTargetReached(FVector TargetLocation, float TargetReachRadius) const
{
	return (TargetLocation - ControlledCharacter->GetActorLocation()).SizeSquared() <= (TargetReachRadius * TargetReachRadius);
}

void AAICharacterController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	TSubclassOf<UAISense> StimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
	if (StimulusClass == UAISense_Damage::StaticClass())
	{
		AActor* MostThreatfulActor = Cast<AActor>(Blackboard->GetValueAsObject(BB_MostDangerousTarget));
		float MostThreatfulActorDamage = Blackboard->GetValueAsFloat(BB_MostDangerousTargetDamage);
		if (Stimulus.IsExpired())
		{
			Blackboard->SetValueAsObject(BB_MostDangerousTarget, nullptr);
			ClearFocus(EAIFocusPriority::Gameplay);
			Blackboard->SetValueAsFloat(BB_MostDangerousTargetDamage, 0); // perhaps redundant
			if (IsValid(Actor))
			{
				Blackboard->SetValueAsVector(BB_InterestingLocation, Actor->GetActorLocation());
			}
		}
		else if (!IsValid(MostThreatfulActor) || MostThreatfulActorDamage < Stimulus.Strength)
		{
			Blackboard->SetValueAsObject(BB_MostDangerousTarget, Actor);
			Blackboard->SetValueAsFloat(BB_MostDangerousTargetDamage, Stimulus.Strength);
		}
	}
	else if (StimulusClass == UAISense_Sight::StaticClass())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsObject(BB_CurrentVisualTarget, Actor);
		}
		else
		{
			Blackboard->SetValueAsObject(BB_CurrentVisualTarget, nullptr);
			ClearFocus(EAIFocusPriority::Gameplay);
			Blackboard->SetValueAsVector(BB_CurrentTargetLastLocation, Actor->GetActorLocation());
		}
	}
	else if (StimulusClass == UAISense_Hearing::StaticClass())
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			Blackboard->SetValueAsVector(BB_InterestingLocation, Stimulus.StimulusLocation);
		}
	}
}