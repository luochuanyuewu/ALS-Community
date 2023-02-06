// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/Notify/ALSNotifyStateEarlyBlendOut.h"
#include "Character/ALSComponent.h"

void UALSNotifyStateEarlyBlendOut::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetAnimInstance())
	{
		return;
	}

	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
	UALSComponent* ALS = UALSComponent::FindALSComponent(MeshComp->GetOwner());
	if (!ALS || !AnimInstance)
	{
		return;
	}

	bool bStopMontage = false;
	if (bCheckMovementState && ALS->GetMovementState() == MovementState)
	{
		bStopMontage = true;
	}
	else if (bCheckStance && ALS->GetStance() == Stance)
	{
		bStopMontage = true;
	}
	else if (bCheckMovementInput && ALS->HasMovementInput())
	{
		bStopMontage = true;
	}

	if (bStopMontage)
	{
		AnimInstance->Montage_Stop(BlendOutTime, ThisMontage);
	}
}

FString UALSNotifyStateEarlyBlendOut::GetNotifyName_Implementation() const
{
	return FString(TEXT("Early Blend Out"));
}

UALSNotifyStateEarlyBlendOut::UALSNotifyStateEarlyBlendOut()
{
	MovementState = FALSGameplayTags::Get().MovementState_None;
	Stance = FALSGameplayTags::Get().Stance_Standing;
}
