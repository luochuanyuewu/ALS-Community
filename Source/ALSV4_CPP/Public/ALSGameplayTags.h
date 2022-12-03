// Copyright 2022 luochuanyuewu. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


/**
 * 库存系统原生标记
 */
struct ALSV4_CPP_API FALSGameplayTags : public FGameplayTagNativeAdder
{
public:
	/*
	 * 库存系统原生标记
	 */
	static const FALSGameplayTags& Get() { return GameplayTags; }
	virtual ~FALSGameplayTags(){};

	FGameplayTag Gait_Walking;
	FGameplayTag Gait_Running;
	FGameplayTag Gait_Sprinting;

	FGameplayTag Stance_Standing;
	FGameplayTag Stance_Crouching;

	FGameplayTag OverlayState_Default;
	FGameplayTag OverlayState_Masculine;
	FGameplayTag OverlayState_Feminine;
	FGameplayTag OverlayState_Injured;
	FGameplayTag OverlayState_HandsTied;
	FGameplayTag OverlayState_Rifle;
	FGameplayTag OverlayState_PistolOneHanded;
	FGameplayTag OverlayState_PistolTwoHanded;
	FGameplayTag OverlayState_Bow;
	FGameplayTag OverlayState_Torch;
	FGameplayTag OverlayState_Binoculars;
	FGameplayTag OverlayState_Box;
	FGameplayTag OverlayState_Barrel;

	FGameplayTag MovementState_None;
	FGameplayTag MovementState_Grounded;
	FGameplayTag MovementState_InAir;
	FGameplayTag MovementState_Mantling;
	FGameplayTag MovementState_Ragdoll;

	FGameplayTag MovementAction_None;
	FGameplayTag MovementAction_LowMantle;
	FGameplayTag MovementAction_HighMantle;
	FGameplayTag MovementAction_Rolling;
	FGameplayTag MovementAction_GettingUp;

	FGameplayTag RotationMode_VelocityDirection;
	FGameplayTag RotationMode_LookingDirection;
	FGameplayTag RotationMode_Aiming;

	FGameplayTag ViewMode_ThirdPerson;
	FGameplayTag ViewMode_FirstPerson;

	FGameplayTag GroundedEntryState_None;
	FGameplayTag GroundedEntryState_Roll;


protected:
	virtual void AddTags() override;
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
private:
	static FALSGameplayTags GameplayTags;
};
