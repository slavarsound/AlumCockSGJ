#include "BasePlayerController.h"

#include "Actors/Equipment/Weapons/ThrowableItem.h"
#include "Components/Character/CharacterAttributesComponent.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/NameplateComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/Movement/BaseCharacterMovementComponent.h"

void ABasePlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	BaseCharacter = Cast<ABaseCharacter>(InPawn);
	if (BaseCharacter.IsValid())
	{
		const UCharacterEquipmentComponent* EquipmentComponent = BaseCharacter->GetEquipmentComponent();
		BaseCharacter->GetCharacterAttributesComponent()->AttributeChangedEvent.AddUObject(this, &ABasePlayerController::OnAttributeChanged);
		
		EquipmentComponent->WeaponAmmoChangedEvent.BindUObject(this, &ABasePlayerController::OnAmmoChanged);
		EquipmentComponent->ThrowableEquippedEvent.BindUObject(this, &ABasePlayerController::OnThrowableEquipped);
		EquipmentComponent->ThrowablesCountChanged.BindUObject(this, &ABasePlayerController::OnThowablesCountChanged);
		EquipmentComponent->WeaponEquippedEvent.AddUObject(this, &ABasePlayerController::OnWeaponEquipped);
		EquipmentComponent->WeaponUnequippedEvent.AddUObject(this, &ABasePlayerController::OnWeaponUnequipped);
		EquipmentComponent->AimStateChangedEvent.AddUObject(this, &ABasePlayerController::OnAimingStateChanged);
		EquipmentComponent->MeleeWeaponEquippedEvent.BindUObject(this, &ABasePlayerController::OnMeleeWeaponEquipped);
	}
}

void ABasePlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (IsValid(PlayerHUDWidgetClass))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
		PlayerHUDWidget->AddToViewport();
	}
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("Turn", this, &ABasePlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ABasePlayerController::LookUp);
	InputComponent->BindAxis("TurnAtRate", this, &ABasePlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpAtRate", this, &ABasePlayerController::LookUpAtRate);
	InputComponent->BindAxis("MoveForward", this, &ABasePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABasePlayerController::MoveRight);

	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABasePlayerController::Mantle);
	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABasePlayerController::Jump);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABasePlayerController::StopSprint);
	
	InputComponent->BindAxis("SwimForward", this, &ABasePlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &ABasePlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &ABasePlayerController ::SwimUp);

	InputComponent->BindAxis("ClimbUp", this, &ABasePlayerController::ClimbUp);
	InputComponent->BindAxis("ClimbDown", this, &ABasePlayerController::ClimbDown);

	InputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayerController::Interact);

	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSliding);
	InputComponent->BindAction("Slide", EInputEvent::IE_Released, this, &ABasePlayerController::StopSliding);
	InputComponent->BindAction("Prone", IE_Pressed, this, &ABasePlayerController::ToggleProneState);
	InputComponent->BindAction("Crouch", IE_Pressed, this, &ABasePlayerController::ToggleCrouchState);

	InputComponent->BindAction("Wallrun", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartWallrun);
	InputComponent->BindAction("Wallrun", EInputEvent::IE_Released, this, &ABasePlayerController::StopWallrun);

	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartFiring);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABasePlayerController::StopFiring);

	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABasePlayerController::StopAiming);

	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABasePlayerController::Reload);

	InputComponent->BindAction("NextWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::PickNextWeapon);
	InputComponent->BindAction("PreviousWeapon", EInputEvent::IE_Pressed, this, &ABasePlayerController::PickPreviousWeapon);

	InputComponent->BindAction("ThrowItem", EInputEvent::IE_Pressed, this, &ABasePlayerController::ThrowItem);

	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartPrimaryMeleeAttack);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Released, this, &ABasePlayerController::StopPrimaryMeleeAttack);
	
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABasePlayerController::StartSecondaryMeleeAttack);
	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Released, this, &ABasePlayerController::StopSecondaryMeleeAttack);

	InputComponent->BindAction("ToggleFireMode", EInputEvent::IE_Pressed, this, &ABasePlayerController::ToggleFireMode);
}

void ABasePlayerController::Interact() 
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->Interact();
	}
}

void ABasePlayerController::ClimbDown(float Value) 
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ClimbDown(Value);
	}
}

void ABasePlayerController::ClimbUp(float Value) 
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ClimbUp(Value);
	}
}

void ABasePlayerController::SwimUp(float Value) 
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->SwimUp(Value);
	}
}

void ABasePlayerController::SwimRight(float Value)
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->SwimRight(Value);
	}
}

void ABasePlayerController::SwimForward(float Value)
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->SwimForward(Value);
	}
}

void ABasePlayerController::TurnAtRate(float Value)
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->TurnAtRate(Value);
	}
}

void ABasePlayerController::LookUpAtRate(float Value)
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->LookUpAtRate(Value);
	}
}

void ABasePlayerController::LookUp(float Value)
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->LookUp(Value);
	}
}

void ABasePlayerController::MoveForward(float Value)
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->MoveForward(Value);
	}
}

void ABasePlayerController::MoveRight(float Value)
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->MoveRight(Value);
	}
}

void ABasePlayerController::Turn(float Value)
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->Turn(Value);
	}
}

void ABasePlayerController::Mantle()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->Mantle();
	}
}

void ABasePlayerController::Jump()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->Jump();
	}
}

void ABasePlayerController::StartSliding()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->TryStartSliding();
	}
}

void ABasePlayerController::StopSliding()
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->StopSliding();
	}
}


void ABasePlayerController::ToggleCrouchState()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ToggleCrouchState();
	}
}

void ABasePlayerController::ToggleProneState()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ToggleProneState();
	}
}

void ABasePlayerController::StartSprint()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartRequestingSprint();
	}
}

void ABasePlayerController::StopSprint()
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->StopRequestingSprint();
	}
}

void ABasePlayerController::StartWallrun()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartRequestingWallrun();
	}
}

void ABasePlayerController::StopWallrun()
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->StopRequestingWallrun();
	}
}

void ABasePlayerController::StartFiring()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartFiring();		
	}
}

void ABasePlayerController::StartAiming()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartAiming();		
	}
}

void ABasePlayerController::StopAiming()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StopAiming();		
	}
}

void ABasePlayerController::Reload()
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->StartReloading();
	}
}

void ABasePlayerController::PickNextWeapon()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ChangeWeapon(1);
	}
}

void ABasePlayerController::PickPreviousWeapon()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ChangeWeapon(-1);
	}
}

void ABasePlayerController::OnAttributeChanged(ECharacterAttribute Attribute, float Percent) const
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->OnAttributeChanged(Attribute, Percent);
	}
}

void ABasePlayerController::ThrowItem()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ThrowItem();
	}
}

void ABasePlayerController::StartPrimaryMeleeAttack()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartPrimaryMeleeAttack();
	}
}

void ABasePlayerController::StopPrimaryMeleeAttack()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StopPrimaryMeleeAttack();
	}
}

void ABasePlayerController::StartSecondaryMeleeAttack()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StartSecondaryMeleeAttack();
	}
}

void ABasePlayerController::StopSecondaryMeleeAttack()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->StopSecondaryMeleeAttack();
	}
}

void ABasePlayerController::ToggleFireMode()
{
	if (BaseCharacter.IsValid() && BaseCharacter->IsMovementInputEnabled())
	{
		BaseCharacter->ToggleFireMode();
	}
}

void ABasePlayerController::StopFiring()
{
	if (BaseCharacter.IsValid())
	{
		BaseCharacter->StopFiring();
	}
}

void ABasePlayerController::OnAimingStateChanged(bool bAiming, ARangeWeaponItem* Weapon)
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->OnAimingStateChanged(bAiming, IsValid(Weapon) ? Weapon->GetReticleType() : EReticleType::None);
	}

	if (IsValid(Weapon) && Weapon->GetAimReticleType() == EReticleType::Scope)
	{
		FViewTargetTransitionParams TransitionParams;
		TransitionParams.BlendTime = 0.2;
		if (bAiming)
		{
			SetViewTarget(Weapon, TransitionParams);
		}
		else
		{
			SetViewTarget(BaseCharacter.Get(), TransitionParams);
		}
	}
	else if (!IsValid(Weapon) && !bAiming && GetViewTarget() != BaseCharacter.Get())
	{
		SetViewTarget(BaseCharacter.Get());
	}
}

void ABasePlayerController::OnAmmoChanged(int32 NewAmmo, int32 TotalAmmo)
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetAmmo(NewAmmo, TotalAmmo);
	}
}

void ABasePlayerController::OnThrowableEquipped(AThrowableItem* Throwable, int32 Count)
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetThrowableName(Throwable->GetNameplateComponent()->GetName());
		PlayerHUDWidget->SetThrowablesCount(Count);
	}
}

void ABasePlayerController::OnThowablesCountChanged(int32 Count)
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetThrowablesCount(Count);
	}
}

void ABasePlayerController::OnWeaponEquipped(const FText& Name, EReticleType Reticle)
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetReticleType(Reticle);
		PlayerHUDWidget->SetWeaponName(Name);
	}
}

void ABasePlayerController::OnWeaponUnequipped()
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->SetReticleType(EReticleType::None);
		PlayerHUDWidget->SetWeaponName(FText::GetEmpty());
	}
}

void ABasePlayerController::OnMeleeWeaponEquipped()
{
	if (IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->OnMeleeWeaponEquipped();
	}
}
