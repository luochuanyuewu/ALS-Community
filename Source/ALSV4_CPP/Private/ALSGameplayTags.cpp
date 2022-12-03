// Copyright 2022 luochuanyuewu. All Rights Reserved.


#include "ALSGameplayTags.h"
#include "GameplayTagsManager.h"

FALSGameplayTags FALSGameplayTags::GameplayTags;


void FALSGameplayTags::AddTags()
{
	AddTag(Gait_Walking, "ALS.Gait.Walking", "");
	AddTag(Gait_Running, "ALS.Gait.Running", "");
	AddTag(Gait_Sprinting, "ALS.Gait.Sprinting", "");

	AddTag(Stance_Standing, "ALS.Stance.Standing", "");
	AddTag(Stance_Crouching, "ALS.Stance.Crouching", "");

	AddTag(OverlayState_Default, "ALS.OverlayState.Default", "");
	AddTag(OverlayState_Masculine, "ALS.OverlayState.Masculine", "");
	AddTag(OverlayState_Feminine, "ALS.OverlayState.Feminine", "");
	AddTag(OverlayState_Injured, "ALS.OverlayState.Injured", "");
	AddTag(OverlayState_HandsTied, "ALS.OverlayState.HandsTied", "");
	AddTag(OverlayState_Rifle, "ALS.OverlayState.Rifle", "");
	AddTag(OverlayState_PistolOneHanded, "ALS.OverlayState.PistolOneHanded", "");
	AddTag(OverlayState_PistolTwoHanded, "ALS.OverlayState.PistolTwoHanded", "");
	AddTag(OverlayState_Bow, "ALS.OverlayState.Bow", "");
	AddTag(OverlayState_Torch, "ALS.OverlayState.Torch", "");
	AddTag(OverlayState_Binoculars, "ALS.OverlayState.Binoculars", "");
	AddTag(OverlayState_Box, "ALS.OverlayState.Box", "");
	AddTag(OverlayState_Barrel, "ALS.OverlayState.Barrel", "");

	AddTag(MovementState_None, "ALS.MovementState.None", "");
	AddTag(MovementState_Grounded, "ALS.MovementState.Grounded", "");
	AddTag(MovementState_InAir, "ALS.MovementState.InAir", "");
	AddTag(MovementState_Mantling, "ALS.MovementState.Mantling", "");
	AddTag(MovementState_Ragdoll, "ALS.MovementState.Ragdoll", "");

	AddTag(MovementAction_None, "ALS.MovementAction.None", "");
	AddTag(MovementAction_LowMantle, "ALS.MovementAction.LowMantle", "");
	AddTag(MovementAction_HighMantle, "ALS.MovementAction.HighMantle", "");
	AddTag(MovementAction_Rolling, "ALS.MovementAction.Rolling", "");
	AddTag(MovementAction_GettingUp, "ALS.MovementAction.GettingUp", "");

	AddTag(RotationMode_VelocityDirection, "ALS.RotationMode.VelocityDirection", "");
	AddTag(RotationMode_LookingDirection, "ALS.RotationMode.LookingDirection", "");
	AddTag(RotationMode_Aiming, "ALS.RotationMode.Aiming", "");

	AddTag(ViewMode_ThirdPerson, "ALS.ViewMode.ThirdPerson", "");
	AddTag(ViewMode_FirstPerson, "ALS.ViewMode.FirstPerson", "");

	AddTag(GroundedEntryState_None, "ALS.GroundedEntryState.None", "");
	AddTag(GroundedEntryState_Roll, "ALS.GroundedEntryState.Roll", "");

	
}

void FALSGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString::Format(TEXT("(库存系统Native) {0}"), {FString(TagComment)}));
}
