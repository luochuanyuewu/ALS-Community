// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/Notify/ALSNotifyStateOverlayOverride.h"

#include "Character/ALSBaseCharacter.h"
#include "Character/ALSComponent.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"

void UALSNotifyStateOverlayOverride::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UALSComponent* ALSComponent = UALSComponent::FindALSComponent(MeshComp->GetOwner());
	if (ALSComponent)
	{
		ALSComponent->SetOverlayOverrideState(OverlayOverrideState);
	}
}

void UALSNotifyStateOverlayOverride::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UALSComponent* ALSComponent = UALSComponent::FindALSComponent(MeshComp->GetOwner());
	if (ALSComponent)
	{
		ALSComponent->SetOverlayOverrideState(0);
	}
}

FString UALSNotifyStateOverlayOverride::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Overlay Override State: "));
	Name.Append(FString::FromInt(OverlayOverrideState));
	return Name;
}
