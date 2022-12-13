// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSBaseCharacter.h"
#include "Character/ALSCharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ALSComponent.h"
#include "Net/UnrealNetwork.h"


AALSBaseCharacter::AALSBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UALSCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = 0;
	bReplicates = true;
	SetReplicatingMovement(true);
}


void AALSBaseCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
}

void AALSBaseCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->OnOwnerCharacterStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
		return;
	}
}

void AALSBaseCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->OnOwnerCharacterEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
		return;
	}
}

void AALSBaseCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->OnOwnerCharacterJumped();
		return;
	}
}

void AALSBaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->OnOwnerCharacterLanded(Hit);
		return;
	}
}
