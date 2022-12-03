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

FGameplayTag MovementStateEnumToTag(EALSMovementState State)
{
	const FALSGameplayTags& Tags = FALSGameplayTags::Get();
	switch (State)
	{
	case EALSMovementState::Ragdoll:
		return Tags.MovementState_Ragdoll;
	case EALSMovementState::InAir:
		return Tags.MovementState_InAir;
	case EALSMovementState::Grounded:
		return Tags.MovementState_Grounded;
	case EALSMovementState::Mantling:
		return Tags.MovementState_Mantling;
	case EALSMovementState::None:
	default:
		return FGameplayTag::EmptyTag;
	}
}

//
// void UALSNotifyStateEarlyBlendOut::PostInitProperties()
// {
// 	Super::PostInitProperties();
// 	bool bModified = false;
// 	if (MovementStateEquals != EALSMovementState::None && MovementState == FALSGameplayTags::Get().MovementState_None)
// 	{
// 		FGameplayTag Tag = MovementStateEnumToTag(MovementStateEquals);
// 		if (Tag.IsValid())
// 		{
// 			MovementState = Tag;
// 			bModified = true;
// 		}
// 	}
//
// 	if (StanceEquals != EALSStance::Standing && Stance == FALSGameplayTags::Get().Stance_Standing)
// 	{
// 		Stance = FALSGameplayTags::Get().Stance_Crouching;
// 		bModified = true;
// 	}
//
// 	if (bModified)
// 	{
// 		MarkPackageDirty();
// 	}
// }
