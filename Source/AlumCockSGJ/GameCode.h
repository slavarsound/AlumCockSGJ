#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCameras, Verbose, All)
#define ECC_Climbable ECC_GameTraceChannel1
#define ECC_Interactable ECC_GameTraceChannel2
#define ECC_Wallrunnable ECC_GameTraceChannel3
#define ECC_Bullet ECC_GameTraceChannel4
#define ECC_MeleeHitRegistrator ECC_GameTraceChannel5

const FName DebugCategoryLedgeDetection = FName("LedgeDetection");
const FName DebugCategoryWallrun = FName("Wallrun");
const FName DebugCategoryAttributes = FName("Attributes");
const FName DebugCategoryRangeWeapons = FName("RangeWeapons");
const FName DebugCategoryMeleeWeapons = FName("MeleeWeapons");

const FName ProfilePawn = FName("Pawn");
const FName ProfileRagdoll = FName("Ragdoll");
const FName ProfileWorldItem = FName("WorldItem");
const FName ProfileInteractionVolume = FName("PawnInteractionVolume");
const FName ProfileProjectile = FName("Projectile");
const FName ProfileNoCollision = FName("NoCollision");

// TODO move it somewhere to be configurable?
const FName SocketPistol = FName("hand_r_pistol_socket"); 
const FName SocketAssaultRifle = FName("hand_r_assault_rifle_socket"); 
const FName SocketForegrip = FName("foregrip_socket");

const FName FXParamTraceEnd = FName("TraceEnd");

const FName BB_CurrentTarget = FName("CurrentTarget");
const FName BB_NextLocation = FName("NextLocation");

