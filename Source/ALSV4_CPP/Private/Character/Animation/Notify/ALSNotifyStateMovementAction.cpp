// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/Notify/ALSNotifyStateMovementAction.h"
#include "Character/ALSComponent.h"

void UALSNotifyStateMovementAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UALSComponent* ALSComponent = UALSComponent::FindALSComponent(MeshComp->GetOwner());
	if (ALSComponent)
	{
		ALSComponent->SetMovementAction(MovementActionTag);
	}
}

void UALSNotifyStateMovementAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                              const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UALSComponent* ALSComponent = UALSComponent::FindALSComponent(MeshComp->GetOwner());

	if (ALSComponent && ALSComponent->GetMovementAction() == MovementActionTag)
	{
		ALSComponent->SetMovementAction(FALSGameplayTags::Get().MovementAction_None);
	}
}

FString UALSNotifyStateMovementAction::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Movement Action: "));

	FString DisplayName = MovementActionTag.ToString();
	DisplayName.RemoveFromStart(TEXT("ALS.MovementAction."));

	Name.Append(DisplayName);
	return Name;
}
