// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/ALSPlayerCameraBehavior.h"
#include "Character/ALSComponent.h"
#include "Character/ALSPlayerCameraManager.h"


void UALSPlayerCameraBehavior::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	AALSPlayerCameraManager* CameraMgr = Cast<AALSPlayerCameraManager>(GetOwningActor());
	if (CameraMgr == nullptr)
		return;
	UALSComponent* ALSComponent = CameraMgr->ALSComponent;
	if (ALSComponent == nullptr)
		return;
	MovementState = ALSComponent->GetMovementState();
	MovementAction = ALSComponent->GetMovementAction();
	ViewMode = ALSComponent->GetViewMode();
	Gait = ALSComponent->GetGait();
	Stance = ALSComponent->GetStance();
	bRightShoulder = ALSComponent->IsRightShoulder();
	
	SetRotationMode(ALSComponent->GetRotationMode());
}

void UALSPlayerCameraBehavior::SetRotationMode(FGameplayTag RotationMode)
{
	bVelocityDirection = RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection;
	bLookingDirection = RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection;
	bAiming = RotationMode == FALSGameplayTags::Get().RotationMode_Aiming;
}
