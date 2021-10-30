#include "BaseCharacter.h"

#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/CapsuleComponent.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Data/Movement/MantlingMovementParameters.h"
#include "Data/Movement/Posture.h"
#include "Data/Movement/StopClimbingMethod.h"
#include "GameCode.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Actors/Interactive/Environment/Ladder.h"
#include "Actors/Interactive/Environment/Zipline.h"
#include "Components/Character/CharacterAttributesComponent.h"
#include "Components/Character/InverseKinematicsComponent.h"
#include "Components/Character/LedgeDetectionComponent.h"
#include "Components/Movement/BaseCharacterMovementComponent.h"
#include "Data/MontagePlayResult.h"
#include "Data/Movement/ZiplineParams.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PhysicsVolume.h"
#include "Kismet/KismetSystemLibrary.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	GCMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
	InverseKinematicsComponent = CreateDefaultSubobject<UInverseKinematicsComponent>(TEXT("InverseKinematics"));
	AddOwnedComponent(InverseKinematicsComponent);
	InverseKinematicsComponent->SetScale(GetActorScale().Z);

	LedgeDetectionComponent = CreateDefaultSubobject<ULedgeDetectionComponent>(TEXT("LedgeDetection"));
	AddOwnedComponent(LedgeDetectionComponent);

	CharacterAttributesComponent = CreateDefaultSubobject<UCharacterAttributesComponent>(TEXT("Attributes"));
	AddOwnedComponent(CharacterAttributesComponent);

	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("Equipment"));
	AddOwnedComponent(CharacterEquipmentComponent);
	
	GCMovementComponent->bNotifyApex = true;

	GetMesh()->CastShadow = true;
	GetMesh()->bCastDynamicShadow = true;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	GCMovementComponent->ClimbableTopReached.BindUObject(this, &ABaseCharacter::OnClimbableTopReached);
	GCMovementComponent->StoppedClimbing.BindUObject(this, &ABaseCharacter::OnStoppedClimbing);
	GCMovementComponent->CrouchedOrProned.BindUObject(this, &ABaseCharacter::OnStartCrouchOrProne);
	GCMovementComponent->WokeUp.BindUObject(this, &ABaseCharacter::OnEndCrouchOrProne);
	GCMovementComponent->SlidingStateChangedEvent.BindUObject(this, &ABaseCharacter::OnSlidingStateChangedEvent);
	GCMovementComponent->ZiplineObstacleHit.BindUObject(this, &ABaseCharacter::OnZiplineObstacleHit);
	GCMovementComponent->WallrunBeginEvent.AddUObject(this, &ABaseCharacter::OnWallrunBegin);
	GCMovementComponent->WallrunEndEvent.AddUObject(this, &ABaseCharacter::OnWallrunEnd);
	
	MontageEndedUnlockControlsEvent.BindUObject(this, &ABaseCharacter::OnMontageEndedUnlockControls);

	CharacterAttributesComponent->OutOfHealthEvent.AddUObject(this, &ABaseCharacter::OnOutOfHealth);
	CharacterAttributesComponent->OutOfStaminaEvent.AddUObject(this, &ABaseCharacter::OnOutOfStamina);

	CharacterEquipmentComponent->AimStateChangedEvent.AddUObject(this, &ABaseCharacter::OnAimStateChanged);
	CharacterEquipmentComponent->ChangingEquippedItemStarted.BindUObject(this, &ABaseCharacter::OnChangingEquippedItemStarted);
	CharacterEquipmentComponent->AimingSpeedChangedEvent.BindUObject(GCMovementComponent, &UBaseCharacterMovementComponent::SetAimingSpeed);
	CharacterEquipmentComponent->AimingSpeedResetEvent.BindUObject(GCMovementComponent, &UBaseCharacterMovementComponent::ResetAimingSpeed);
	
	OnTakeAnyDamage.AddDynamic(CharacterAttributesComponent, &UCharacterAttributesComponent::OnTakeAnyDamage);
	OnTakeAnyDamage.AddDynamic(this, &ABaseCharacter::ReactToDamage);

	CharacterEquipmentComponent->CreateLoadout();
	UpdateStrafingControls();
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TryChangeSprintState();
	const EPosture CurrentPosture = GCMovementComponent->GetCurrentPosture();
	if (GCMovementComponent->IsMovingOnGround() && (CurrentPosture == EPosture::Standing || CurrentPosture == EPosture::Crouching))
	{
		InverseKinematicsComponent->CalculateIkData(GetMesh(), GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
			GetActorLocation(), bIsCrouched, DeltaTime);
	}

	UpdateSuffocatingState();
}

#pragma region ATTRIBUTES

void ABaseCharacter::OnOutOfHealth()
{
	bool bDeathMontagePlaying = false;
	// StopAnimMontage();
	if (GCMovementComponent->IsMovingOnGround())
	{
		const float Duration = PlayAnimMontage(DeathAnimationMontage);
		bDeathMontagePlaying = Duration > 0.f;
	}

	GCMovementComponent->DisableMovement();
	SetInputDisabled(true, true);
	if (!bDeathMontagePlaying)
	{
		EnableRagdoll();
	}
}

void ABaseCharacter::UpdateSuffocatingState()
{
	if (GCMovementComponent->IsSwimming())
	{
		FVector HeadPosition = GetMesh()->GetBoneLocation(HeadBoneName);
		APhysicsVolume* Volume = GCMovementComponent->GetPhysicsVolume();
		if (IsValid(Volume))
		{
			float WaterPlaneZ = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z * Volume->GetActorScale3D().Z;
			CharacterAttributesComponent->SetSuffocating(HeadPosition.Z < WaterPlaneZ);
		}
	}
}

void ABaseCharacter::OnOutOfStamina(bool bOutOfStamina)
{
	if (bOutOfStamina)
	{
		if (GCMovementComponent->IsSprinting())
		{
			StopSprinting();
			SetStrafingControlsState(false);
		}
		else if (GCMovementComponent->IsWallrunning())
		{
			StopWallrunning();
		}
	}
	else
	{
		UpdateStrafingControls();
	}
	
	GCMovementComponent->SetIsOutOfStamina(bOutOfStamina);
}

#pragma endregion ATTRIBUTES

#pragma region SPRINT

void ABaseCharacter::StartRequestingSprint()
{
	bSprintRequested = true;
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABaseCharacter::StopRequestingSprint()
{
	bSprintRequested = false;
}

bool ABaseCharacter::CanSprint() const
{
	return IsPendingMovement()
		&& !CharacterEquipmentComponent->IsAiming()
		&& GCMovementComponent->IsMovingOnGround() && !CharacterAttributesComponent->IsOutOfStamina()
		&& GCMovementComponent->GetCurrentPosture() != EPosture::Proning;
}

void ABaseCharacter::TryStartSprinting()
{
	bool bSprintStarted = GCMovementComponent->TryStartSprint();
	if (bSprintStarted && CharacterEquipmentComponent->GetEquippedItemType() != EEquippableItemType::None)
	{
		SetStrafingControlsState(false);
	}
	
	CharacterAttributesComponent->SetSprinting(bSprintStarted);
	OnSprintStart();
}

void ABaseCharacter::StopSprinting()
{
	if (!GCMovementComponent->IsSprinting())
	{
		return;
	}
	
	GCMovementComponent->StopSprint();
	if (GCMovementComponent->IsMovingOnGround() && CharacterEquipmentComponent->IsPreferStrafing())
	{
		SetStrafingControlsState(true);
	}
	
	CharacterAttributesComponent->SetSprinting(false);
	OnSprintEnd();
}

void ABaseCharacter::TryChangeSprintState()
{
	if (bSprintRequested && !GCMovementComponent->IsSprinting() && CanSprint())
	{
		TryStartSprinting();
	}
	else if ((!bSprintRequested || !CanSprint()) && GCMovementComponent->IsSprinting())
	{
		StopSprinting();
	}
}

#pragma endregion 

#pragma region JUMP/FALL

void ABaseCharacter::Jump()
{
	if (GCMovementComponent->GetCurrentPosture() == EPosture::Proning)
	{
		GCMovementComponent->RequestStandUp();
	}
	else if (bIsCrouched)
	{
		UnCrouch();
	}
	else if (GCMovementComponent->IsClimbing())
	{
		GCMovementComponent->StopClimbing(EStopClimbingMethod::JumpOff);
	}
	else if (GCMovementComponent->IsZiplining())
	{
		StopZiplining();
	}
	else if (GCMovementComponent->IsWallrunning())
	{
		GCMovementComponent->JumpOffWall();
	}
	else
	{
		if (GCMovementComponent->IsSliding())
		{
			StopSliding();
		}
		
		Super::Jump();
	}
}

void ABaseCharacter::OnJumped_Implementation()
{
	CharacterAttributesComponent->OnJumped();
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation()
		&& !CharacterAttributesComponent->IsOutOfStamina()
		&& !GCMovementComponent->IsMantling()
		&& !GCMovementComponent->IsProning();
}

void ABaseCharacter::Falling()
{
	if (GCMovementComponent->IsSprinting())
	{
		StopSprinting();
	}

	CharacterAttributesComponent->SetFalling(true);
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	GCMovementComponent->bNotifyApex = 1;
	const FVector Velocity = GCMovementComponent->Velocity;

	if (Hit.bBlockingHit && -Velocity.Z > HardLandVelocityZ)
	{
		const float SurfaceNormalZToStopSliding = 0.9f;
		if (GCMovementComponent->IsSliding() && Hit.ImpactNormal.Z > SurfaceNormalZToStopSliding)
		{
			StopSliding(true);
		}

		FMontagePlayResult MontagePlayResult = PlayHardLandMontage();
		if (MontagePlayResult.bStarted)
		{
			GCMovementComponent->Velocity = FVector::ZeroVector;
			SetInputDisabled(true, MontagePlayResult.bDisableCameraRotation);
			MontagePlayResult.AnimInstance->Montage_SetEndDelegate(MontageEndedUnlockControlsEvent, MontagePlayResult.AnimMontage);	
		}
	}

	if (IsValid(FallDamageCurve))
	{
		float FallDamage = FallDamageCurve->GetFloatValue(-Velocity.Z);
		if (FallDamage > 0.f)
		{
			TakeDamage(FallDamage, FDamageEvent(), GetController(), Hit.Actor.Get());
		}
	}
	
	CharacterAttributesComponent->SetFalling(false);
}

void ABaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	FallApexWorldZ = GetActorLocation().Z;
}

FMontagePlayResult ABaseCharacter::PlayHardLandMontage()
{
	return PlayHardLandMontage(GetMesh()->GetAnimInstance(), HardLandMontageTP);
}

FMontagePlayResult ABaseCharacter::PlayHardLandMontage(UAnimInstance* AnimInstance, UAnimMontage* AnimMontage) const
{
	FMontagePlayResult MontagePlayResult;
	MontagePlayResult.bStarted = false;
	if (IsValid(AnimInstance))
	{
		if (IsValid(AnimMontage))
		{
			MontagePlayResult.bStarted = AnimInstance->Montage_Play(AnimMontage, 1, EMontagePlayReturnType::Duration, 0.f) > 0;
			if (MontagePlayResult.bStarted)
			{
				MontagePlayResult.AnimInstance = AnimInstance;
				MontagePlayResult.AnimMontage = AnimMontage;
			}
		}
	}

	return MontagePlayResult;
}

#pragma endregion JUMP/FALL

#pragma region INTERACTION

void ABaseCharacter::Interact()
{
	if (bInteracting)
	{
		TryStopInteracting();
	}
	else
	{
		TryStartInteracting();
	}
}

void ABaseCharacter::TryStopInteracting()
{
	if (IsValid(CurrentInteractable))
	{
		switch (CurrentInteractable->GetInteractionType())
		{
			case EInteractionType::Climbing:
				GCMovementComponent->StopClimbing();
				break;
			case EInteractionType::Ziplining:
				break;
			default:
				return;
		}	
	}

	ResetInteraction();
}

// TODO InteractionComponent
bool ABaseCharacter::TryStartInteracting()
{
	const AInteractiveActor* Interactable = GetPreferableInteractable();
	if (!IsValid(Interactable))
	{
		return false;
	}

	bool bInteractionStarted = false;
	switch (Interactable->GetInteractionType())
	{
		case EInteractionType::Climbing:
		{
			const ALadder* Ladder = StaticCast<const ALadder*>(Interactable);

			if (Ladder->IsOnTop())
			{
				PlayAnimMontage(Ladder->GetAttachFromTopAnimMontage());
			}
	
			bInteractionStarted = GCMovementComponent->TryStartClimbing(Ladder);
			break;
		}
		case EInteractionType::Ziplining:
		{
			const AZipline* Zipline = StaticCast<const AZipline*>(Interactable);
			FZiplineParams ZiplineParams = GetZipliningParameters(Zipline);
			bInteractionStarted = GCMovementComponent->TryStartZiplining(ZiplineParams);
			break;
		}
		default:
			break;
	}

	if (bInteractionStarted)
	{
		bInteracting = true;
		CurrentInteractable = Interactable;	
	}

	return bInteractionStarted;
}

void ABaseCharacter::RegisterInteractiveActor(const AInteractiveActor* InteractiveActor)
{
	InteractiveActor->Tags;
	InteractiveActors.AddUnique(InteractiveActor);
}

void ABaseCharacter::UnregisterInteractiveActor(const AInteractiveActor* InteractiveActor)
{
	if (InteractiveActor != CurrentInteractable)
		InteractiveActors.RemoveSingleSwap(InteractiveActor);
}

const AInteractiveActor* ABaseCharacter::GetPreferableInteractable()
{
	auto InteractablesCount = InteractiveActors.Num();	
	if (InteractablesCount <= 0)
		return nullptr;

	const AInteractiveActor* Interactable = nullptr;
	while (InteractablesCount > 0)
	{
		Interactable = InteractiveActors[0];
		if (IsValid(Interactable))
			break;
		
		InteractiveActors.RemoveSingleSwap(CurrentInteractable);
		Interactable = nullptr;
		InteractablesCount--;
	}
	
	return Interactable;
}

void ABaseCharacter::ResetInteraction()
{
	bInteracting = false;
	CurrentInteractable = nullptr;
}

#pragma endregion INTERACTION

#pragma region CROUCH/PRONE

void ABaseCharacter::ToggleCrouchState()
{
	if (!bIsCrouched && !GCMovementComponent->IsSliding())
	{
		GCMovementComponent->RequestCrouch();
	}
}

void ABaseCharacter::ToggleProneState()
{
	if (bIsCrouched)
	{
		GCMovementComponent->RequestProne();
	}
}

bool ABaseCharacter::CanCrouch() const
{
	return Super::CanCrouch() && !GCMovementComponent->IsSwimming();
}

void ABaseCharacter::OnStartCrouchOrProne(float HalfHeightAdjust)
{
	UpdateStrafingControls();
	ChangeMeshOffset(HalfHeightAdjust);
}

void ABaseCharacter::OnEndCrouchOrProne(float HalfHeightAdjust)
{
	UpdateStrafingControls();
	ChangeMeshOffset(-HalfHeightAdjust);
}

#pragma endregion CROUCH/PRONE

#pragma region ZIPLINE

void ABaseCharacter::OnZiplineObstacleHit(FHitResult Hit)
{
	if (Hit.bBlockingHit)
	{
		StopZiplining();
	}
}

void ABaseCharacter::StopZiplining()
{
	GCMovementComponent->StopZiplining();
	ResetInteraction();
}

FZiplineParams ABaseCharacter::GetZipliningParameters(const AZipline* Zipline) const
{
	FZiplineParams ZiplineParams;
	ZiplineParams.Friction = Zipline->GetCableFriction();
	ZiplineParams.ZiplineNormalizedDirection = Zipline->GetZiplineDirection().GetSafeNormal();
	ZiplineParams.DeclinationAngle = Zipline->GetDeclinationAngle();
	const FVector TopPoleLocation = Zipline->GetTopPoleWorldLocation();
	const FVector BottomPoleLocation = Zipline->GetBottomPoleWorldLocation();
	const FVector TopPoleProjection = FVector(TopPoleLocation.X, TopPoleLocation.Y, BottomPoleLocation.Z); 	
	ZiplineParams.MovementPlane = FPlane(TopPoleLocation,BottomPoleLocation, TopPoleProjection);
				
	const float ZiplineDistance = (TopPoleLocation - BottomPoleLocation).Size();
	const FVector& ActorLocation = GetActorLocation();
	FVector ProjectedVelocity = FVector::VectorPlaneProject(GetVelocity(), ZiplineParams.MovementPlane.GetNormal());
	ZiplineParams.CurrentSpeed = ProjectedVelocity.Size();
	
	ZiplineParams.DeclinationAngleSin = FMath::Sin(FMath::DegreesToRadians(ZiplineParams.DeclinationAngle));
	ZiplineParams.DeclinationAngleCos = FMath::Cos(FMath::DegreesToRadians(ZiplineParams.DeclinationAngle));
	
	FVector SocketOffset = GetMesh()->GetSocketTransform(ZiplineHandPositionSocketName,RTS_Actor).GetLocation();
	const float CharacterToZiplineDistance = (TopPoleLocation - ActorLocation).Size();
	FVector NewActorLocation = TopPoleLocation
		+ (BottomPoleLocation - TopPoleLocation) * CharacterToZiplineDistance / ZiplineDistance;
	float RotationHeightDeltaAngle = (180.f - ZiplineParams.DeclinationAngle) * 0.5f;
	const UCapsuleComponent* CharacterCapsule = GetCapsuleComponent();
	const float HalfHeightAdjustment = (CharacterCapsule->GetScaledCapsuleHalfHeight() + CharacterCapsule->GetScaledCapsuleRadius())
	* ZiplineParams.DeclinationAngleSin * (1.f / FMath::Tan(FMath::DegreesToRadians(RotationHeightDeltaAngle)));
	ZiplineParams.CorrectedActorLocation = NewActorLocation - SocketOffset - HalfHeightAdjustment * FVector::UpVector;
	return ZiplineParams;
}

#pragma endregion ZIPLINE

#pragma region MANTLE

void ABaseCharacter::Mantle(bool bForce)
{
	if (!CanMantle() && !bForce)
	{
		return;
	}
	
	FLedgeDescriptor LedgeDescriptor;
	if (LedgeDetectionComponent->DetectLedge(LedgeDescriptor))
	{
		const auto ActorTransform = GetActorTransform();
		const auto MantleSettings = GetMantlingSettings(LedgeDescriptor.MantlingHeight);
		FMantlingMovementParameters MantleParams(LedgeDescriptor, ActorTransform, MantleSettings);
		if (bIsCrouched)
		{
			const float DefaultHalfHeight = GetDefaultHalfHeight() * GetActorScale().Z;
			MantleParams.InitialLocation.Z += DefaultHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}
		
		bool bMantleStarted = GCMovementComponent->TryStartMantle(MantleParams);
		if (!bMantleStarted)
		{
			return;
		}
		
		if (bIsCrouched)
		{
			UnCrouch();
		}
		
		PlayMantleMontage(MantleSettings, MantleParams.StartTime);
	}
}

void ABaseCharacter::PlayMantleMontage(const FMantlingSettings& MantleSettings, float StartTime)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance) && IsValid(MantleSettings.MantleMontageTP))
	{
		AnimInstance->Montage_Play(MantleSettings.MantleMontageTP, 1, EMontagePlayReturnType::Duration, StartTime);
	}
}

const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float Height) const
{
	return Height <= MantleLowMaxHeight ? MantleLowSettings : MantleHighSettings;
}

bool ABaseCharacter::CanMantle() const
{
	return !GCMovementComponent->IsMantling()
		&& (GCMovementComponent->GetCurrentPosture() == EPosture::Standing
			|| GCMovementComponent->GetCurrentPosture() == EPosture::Crouching)
		&& !GCMovementComponent->IsWallrunning()
		&& IsValid(MantleHighSettings.MantleCurve) && IsValid(MantleLowSettings.MantleCurve);
}

#pragma endregion MANTLE

#pragma region CLIMB

void ABaseCharacter::OnClimbableTopReached()
{
	Mantle(true);
	InteractiveActors.RemoveSingleSwap(CurrentInteractable);
	ResetInteraction();
}

void ABaseCharacter::OnStoppedClimbing(const AInteractiveActor* Interactable)
{
	if (bInteracting && Interactable == CurrentInteractable)
	{
		// TODO refactor so that these fields are updated in a single method
		ResetInteraction();
	}
}

#pragma endregion CLIMB

#pragma region WALLRUN

void ABaseCharacter::StartRequestingWallrun()
{	
	if (CanAttemptWallrun())
	{
		GCMovementComponent->RequestWallrunning();
	}
}

void ABaseCharacter::StopRequestingWallrun()
{
	GCMovementComponent->StopWallrunning(false);
}

bool ABaseCharacter::CanAttemptWallrun() const
{
	return IsPendingMovement()
		&& !CharacterAttributesComponent->IsOutOfStamina()
		&& (GCMovementComponent->IsMovingOnGround() || GCMovementComponent->IsFalling())
		&& GCMovementComponent->GetCurrentPosture() == EPosture::Standing;
}

void ABaseCharacter::OnWallrunBegin(ESide WallrunSide)
{
	CharacterAttributesComponent->SetWallrunning(true);
}

void ABaseCharacter::OnWallrunEnd(ESide WallrunSide) const
{
	CharacterAttributesComponent->SetWallrunning(false);
}

void ABaseCharacter::StopWallrunning()
{
	GCMovementComponent->StopWallrunning(false);
	CharacterAttributesComponent->SetWallrunning(false);
}

#pragma endregion WALLRUN

#pragma region SLIDE

void ABaseCharacter::TryStartSliding()
{
	if (CanSlide())
	{
		bool bSlideStarted = GCMovementComponent->TryStartSliding();
		if (bSlideStarted)
		{
			StopSprinting();
		}
	}
}

void ABaseCharacter::StopSliding(bool bForce)
{
	if (bForce || GCMovementComponent->IsSliding())
	{
		GCMovementComponent->StopSliding();
	}
}

void ABaseCharacter::OnSlidingStateChangedEvent(bool bSliding, float HalfHeightAdjust)
{
	ChangeMeshOffset(bSliding ? HalfHeightAdjust : -HalfHeightAdjust);
}

#pragma endregion SLIDE

#pragma region WEAPONS

bool ABaseCharacter::CanShoot() const
{
	return !(GCMovementComponent->IsSwimming() || GCMovementComponent->IsClimbing()
		|| GCMovementComponent->IsZiplining() || GCMovementComponent->IsWallrunning()
		|| GCMovementComponent->IsMantling());
}

void ABaseCharacter::StartFiring()
{
	if (CharacterAttributesComponent->IsAlive() && CanShoot())
	{
		CharacterEquipmentComponent->StartShooting(Controller);
	}
}

void ABaseCharacter::StopFiring()
{
	CharacterEquipmentComponent->StopFiring();
}

void ABaseCharacter::StartAiming()
{
	CharacterEquipmentComponent->StartAiming();
}

void ABaseCharacter::StopAiming()
{
	CharacterEquipmentComponent->StopAiming();
}

void ABaseCharacter::StartReloading()
{
	if (CanReload())
	{
		CharacterEquipmentComponent->TryReload();
	}
}

void ABaseCharacter::ChangeWeapon(int WeaponIndexDelta)
{
	CharacterEquipmentComponent->EquipWeapon(WeaponIndexDelta);
}

void ABaseCharacter::ToggleFireMode()
{
	CharacterEquipmentComponent->StartTogglingFireMode();
}

void ABaseCharacter::ThrowItem()
{
	CharacterEquipmentComponent->TryThrow();
}

bool ABaseCharacter::CanReload() const
{
	return GCMovementComponent->IsMovingOnGround() || GCMovementComponent->IsFalling() || GCMovementComponent->IsProning();
}

void ABaseCharacter::OnChangingEquippedItemStarted(UAnimMontage* AnimMontage, float Duration)
{
	PlayAnimMontageWithDuration(AnimMontage, Duration);
}

void ABaseCharacter::OnAimStateChanged(bool bAiming, ARangeWeaponItem* Weapon)
{
	GCMovementComponent->SetIsAiming(bAiming);
	if (bAiming && IsValid(Weapon))
	{
		OnAimingStart(Weapon->GetAimFOV(), Weapon->GetAimTurnModifier(), Weapon->GetAimLookUpModifier());
	}
	else
	{
		OnAimingEnd();
	}
}

#pragma endregion WEAPONS

#pragma region MELEE

void ABaseCharacter::StartPrimaryMeleeAttack()
{
	CharacterEquipmentComponent->StartPrimaryMeleeAttack(Controller);
}

void ABaseCharacter::StopPrimaryMeleeAttack()
{
	CharacterEquipmentComponent->StopPrimaryMeleeAttack();
}

void ABaseCharacter::StartSecondaryMeleeAttack()
{
	CharacterEquipmentComponent->StartHeavyMeleeAttack(Controller);
}

void ABaseCharacter::StopSecondaryMeleeAttack()
{
	CharacterEquipmentComponent->StopHeavyMeleeAttack();
}

#pragma endregion MELEE

#pragma region MOVEMENT

void ABaseCharacter::MoveForward(float Value)
{
	CurrentInputForward = Value;
	GCMovementComponent->SetForwardInput(Value);
}

void ABaseCharacter::SetInputDisabled(bool bDisabledMovement, bool bDisabledCamera)
{
	if (IsValid(Controller))
	{
		Controller->SetIgnoreLookInput(bDisabledCamera);
		Controller->SetIgnoreMoveInput(bDisabledMovement);
		bMovementInputEnabled = !bDisabledMovement;
		if (bDisabledMovement)
		{
			CurrentInputForward = 0.f;
			CurrentInputRight = 0.f;
		}
	}
}

void ABaseCharacter::OnMontageEndedUnlockControls(UAnimMontage* AnimMontage, bool bInterrupted)
{
	SetInputDisabled(false, false);
	UpdateStrafingControls();
}

void ABaseCharacter::OnMantleEnded()
{
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
}

void ABaseCharacter::EnableRagdoll() const
{
	GetMesh()->SetCollisionProfileName(ProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

void ABaseCharacter::UpdateStrafingControls()
{
	SetStrafingControlsState(GCMovementComponent->IsMovingOnGround() && CharacterEquipmentComponent->IsPreferStrafing()
		&& !CharacterAttributesComponent->IsOutOfStamina()
		&& (GCMovementComponent->GetCurrentPosture() == EPosture::Standing || GCMovementComponent->GetCurrentPosture() == EPosture::Crouching)
		&& !GCMovementComponent->IsSprinting());
}

void ABaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	CharacterAttributesComponent->SetMovementMode(GCMovementComponent->MovementMode);
	switch (PrevMovementMode)
	{
		case EMovementMode::MOVE_Swimming:
			CharacterAttributesComponent->SetSuffocating(false);
			break;
		case EMovementMode::MOVE_Falling:
		case EMovementMode::MOVE_Flying:
			CharacterAttributesComponent->SetFalling(false);
			break;
		default:
			break;
	}

	UpdateStrafingControls();
}

void ABaseCharacter::SetStrafingControlsState(bool bStrafing)
{
	GCMovementComponent->bOrientRotationToMovement = !bStrafing;
	bUseControllerRotationYaw = bStrafing;
}

#pragma endregion MOVEMENT

void ABaseCharacter::ChangeMeshOffset(float HalfHeightAdjust)
{
	RecalculateBaseEyeHeight();
	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z += HalfHeightAdjust;
	BaseTranslationOffset.Z = MeshRelativeLocation.Z;
}

float ABaseCharacter::PlayAnimMontageWithDuration(UAnimMontage* Montage, float DesiredDuration)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	const float PlayRate = Montage->GetPlayLength() / DesiredDuration;
	return AnimInstance->Montage_Play(Montage, PlayRate);
}

void ABaseCharacter::ReactToDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CharacterAttributesComponent->IsAlive())
	{
		return;
	}
	
	if (HitReactionMontages.Num() > 0)
	{
		UAnimMontage* HitReactionMontage = HitReactionMontages[FMath::RandRange(0, HitReactionMontages.Num() -1)];
		PlayAnimMontage(HitReactionMontage);	
	}
}

bool ABaseCharacter::CanStartAction(ECharacterAction Action)
{
	if (!CharacterAttributesComponent->IsAlive())
	{
		return false;
	}
	
	const FCharacterActions* BlockedByActions = ActionBlockers.Find(Action);
	if (!BlockedByActions || BlockedByActions->AffectedActions.Num() == 0)
	{
		return true;
	}

	return BlockedByActions->AffectedActions.Intersect(ActiveActions).Num() == 0;
}

void ABaseCharacter::OnActionStarted(ECharacterAction Action)
{
	ActiveActions.Add(Action);
	const FCharacterActions* ActionsToInterrupt = ActionInterrupters.Find(Action);
	if (!ActionsToInterrupt)
	{
		return;
	}

	TSet<ECharacterAction> AffectedActions = ActionsToInterrupt->AffectedActions.Intersect(ActiveActions);
	for (ECharacterAction ActionToInterrupt : AffectedActions)
	{
		switch (ActionToInterrupt)
		{
			case ECharacterAction::Aim:
				CharacterEquipmentComponent->StopAiming();
				break;
			case ECharacterAction::Sprint:
				StopSprinting();
				break;
			case ECharacterAction::Climb:
			case ECharacterAction::Zipline:
				TryStopInteracting();
				break;
			case ECharacterAction::Wallrun:
				GCMovementComponent->StopWallrunning(false);
			break;
			case ECharacterAction::Crawl:
			case ECharacterAction::Crouch:
				GCMovementComponent->UnCrouch();
				break;
			case ECharacterAction::Slide:
				GCMovementComponent->StopSliding();
			break;
			case ECharacterAction::Reload:
				CharacterEquipmentComponent->InterruptReloading();
				break;
			case ECharacterAction::Shoot:
				CharacterEquipmentComponent->StopFiring();
			break;
			case ECharacterAction::ChangeEquipment:
				CharacterEquipmentComponent->InterruptChangingEquipment();
			break;
			case ECharacterAction::ThrowItem:
				CharacterEquipmentComponent->InterruptThrowingItem();
			break;
			case ECharacterAction::MeleeAttack:
				// CharacterEquipmentComponent->StopMeleeAttack();
			break;
			case ECharacterAction::HardLand:
			case ECharacterAction::Swim:
			case ECharacterAction::Mantle:
			case ECharacterAction::Jump:
			default:
				// these are uninterruptable
				break;
		}
	}
}

void ABaseCharacter::OnActionEnded(ECharacterAction Action)
{
	ActiveActions.Remove(Action);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// AAIController* AIController = Cast<AAIController>(NewController);
	IGenericTeamAgentInterface* AIController = Cast<IGenericTeamAgentInterface>(NewController);
	if (!AIController)
	{
		return;
	}

	AIController->SetGenericTeamId(FGenericTeamId((uint8)Team));
}
