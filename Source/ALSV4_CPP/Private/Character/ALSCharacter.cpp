// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSCharacter.h"

#include "Engine/StaticMesh.h"
#include "AI/ALSAIController.h"
#include "Character/ALSComponent.h"
#include "Kismet/GameplayStatics.h"

AALSCharacter::AALSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeldObjectRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeldObjectRoot"));
	HeldObjectRoot->SetupAttachment(GetMesh());

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(HeldObjectRoot);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(HeldObjectRoot);

	AIControllerClass = AALSAIController::StaticClass();
}

void AALSCharacter::ClearHeldObject_Implementation()
{
	StaticMesh->SetStaticMesh(nullptr);
	SkeletalMesh->SetSkeletalMesh(nullptr);
	SkeletalMesh->SetAnimInstanceClass(nullptr);
}

void AALSCharacter::AttachToHand(UStaticMesh* NewStaticMesh, USkeletalMesh* NewSkeletalMesh, UClass* NewAnimClass,
                                 bool bLeftHand, FVector Offset)
{
	ClearHeldObject();

	if (IsValid(NewStaticMesh))
	{
		StaticMesh->SetStaticMesh(NewStaticMesh);
	}
	else if (IsValid(NewSkeletalMesh))
	{
		SkeletalMesh->SetSkeletalMesh(NewSkeletalMesh);
		if (IsValid(NewAnimClass))
		{
			SkeletalMesh->SetAnimInstanceClass(NewAnimClass);
		}
	}

	FName AttachBone;
	if (bLeftHand)
	{
		AttachBone = TEXT("VB LHS_ik_hand_gun");
	}
	else
	{
		AttachBone = TEXT("VB RHS_ik_hand_gun");
	}

	HeldObjectRoot->AttachToComponent(GetMesh(),
	                                  FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachBone);
	HeldObjectRoot->SetRelativeLocation(Offset);
}


void AALSCharacter::OnOverlayStateChanged(FGameplayTag PreviousState)
{
	UpdateHeldObject();
}

void AALSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateHeldObjectAnimations();
}

void AALSCharacter::BeginPlay()
{
	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->RagdollStateChangedDelegate.AddDynamic(this,&ThisClass::OnRagdollStateChanged);
		ALSComponent->OnOverlayStateChangedEvent.AddDynamic(this, &ThisClass::OnOverlayStateChanged);
	}

	Super::BeginPlay();
	
	UpdateHeldObject();
}

void AALSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UALSComponent* ALSComponent = UALSComponent::FindALSComponent(this))
	{
		ALSComponent->RagdollStateChangedDelegate.RemoveDynamic(this,&ThisClass::OnRagdollStateChanged);
		ALSComponent->OnOverlayStateChangedEvent.RemoveDynamic(this, &ThisClass::OnOverlayStateChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void AALSCharacter::OnRagdollStateChanged(bool bRagdollState)
{
	if (bRagdollState)
	{
		ClearHeldObject();
	}else
	{
		UpdateHeldObject();
	}
}
