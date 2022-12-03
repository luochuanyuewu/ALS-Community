// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/Animation/Notify/ALSAnimNotifyGroundedEntryState.h"

#include "Character/ALSBaseCharacter.h"
#include "Character/ALSComponent.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"

void UALSAnimNotifyGroundedEntryState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);


	if (UALSComponent* ALS = UALSComponent::FindALSComponent(MeshComp->GetOwner()))
	{
		ALS->SetGroundedEntryState(GroundedEntryState);
	}

}

FString UALSAnimNotifyGroundedEntryState::GetNotifyName_Implementation() const
{
	FString Name(TEXT("Grounded Entry State: "));
	Name.Append(GroundedEntryState.ToString());
	return Name;
}

