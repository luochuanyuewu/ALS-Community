// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#include "Character/ALSComponent.h"
#include "ALSGameplayTags.h"
#include "ALSSettings.h"
#include "Character/Animation/ALSCharacterAnimInstance.h"
#include "Library/ALSMathLibrary.h"
#include "Components/ALSDebugComponent.h"
#include "Components/CapsuleComponent.h"
#include "Curves/CurveFloat.h"
#include "Character/ALSCharacterMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"



UALSComponent::UALSComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	// bUseControllerRotationYaw = 0;
	SetIsReplicatedByDefault(true);
	// SetReplicatingMovement(true);

	const FALSGameplayTags& ALSTags = FALSGameplayTags::Get();
	DesiredRotationMode = ALSTags.RotationMode_LookingDirection;
	DesiredGait = ALSTags.Gait_Running;
	DesiredStance = ALSTags.Stance_Standing;

	OverlayState = ALSTags.OverlayState_Default;

	MovementState = ALSTags.MovementState_None;
	PrevMovementState = ALSTags.MovementState_None;

	MovementAction = ALSTags.MovementAction_None;

	RotationMode = ALSTags.RotationMode_LookingDirection;

	Gait = ALSTags.Gait_Running;

	Stance = ALSTags.Stance_Standing;

	ViewMode = ALSTags.ViewMode_ThirdPerson;
}

// void UALSCharacterComponent::PostInitializeComponents()
// {
// 	Super::PostInitializeComponents();
// 	MyCharacterMovementComponent = Cast<UALSCharacterMovementComponent>(Super::GetMovementComponent());
// }

void UALSComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TargetRagdollLocation);
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedCurrentAcceleration, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, ReplicatedControlRotation, COND_SkipOwner);

	DOREPLIFETIME(ThisClass, DesiredGait);
	DOREPLIFETIME_CONDITION(ThisClass, DesiredStance, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, DesiredRotationMode, COND_SkipOwner);

	DOREPLIFETIME_CONDITION(ThisClass, RotationMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, OverlayState, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, ViewMode, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, VisibleMesh, COND_SkipOwner);
}

void UALSComponent::OnBreakfall_Implementation()
{
	Replicated_PlayMontage(GetRollAnimation(), 1.35);
}

void UALSComponent::Replicated_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	// Roll: Simply play a Root Motion Montage.
	if (OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	Server_PlayMontage(Montage, PlayRate);
}

void UALSComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!OwnerCharacter)
	{
		OwnerCharacter = Cast<ACharacter>(GetOwner());
	}

	if (OwnerCharacter->bUseControllerRotationYaw)
	{
		// TODO Log
		OwnerCharacter->bUseControllerRotationYaw = false;
	}
	
	OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this,&ThisClass::OnMovementModeChanged);
	OnMovementModeChanged(OwnerCharacter,OwnerCharacter->GetCharacterMovement()->GetGroundMovementMode(),0);
	
	// If we're in networked game, disable curved movement
	bEnableNetworkOptimizations = !IsNetMode(NM_Standalone);

	// Make sure the mesh and animbp update after the CharacterBP to ensure it gets the most recent values.
	OwnerCharacter->GetMesh()->AddTickPrerequisiteActor(OwnerCharacter);

	// Set the Movement Model
	SetMovementModel(MovementModel);

	// Force update states to use the initial desired values.
	ForceUpdateCharacterState();

	if (Stance == FALSGameplayTags::Get().Stance_Standing)
	{
		OwnerCharacter->UnCrouch();
	}
	else if (Stance == FALSGameplayTags::Get().Stance_Crouching)
	{
		OwnerCharacter->Crouch();
	}

	// Set default rotation values.
	TargetRotation = OwnerCharacter->GetActorRotation();
	LastVelocityRotation = TargetRotation;
	LastMovementInputRotation = TargetRotation;

	if (OwnerCharacter->GetMesh()->GetAnimInstance() && GetOwnerRole() == ROLE_SimulatedProxy)
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->SetRootMotionMode(ERootMotionMode::IgnoreRootMotion);
	}

	if (MyCharacterMovementComponent)
	{
		MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}

	ALSDebugComponent = GetOwner()->FindComponentByClass<UALSDebugComponent>();
}

void UALSComponent::OnRegister()
{
	Super::OnRegister();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		MyCharacterMovementComponent = Cast<UALSCharacterMovementComponent>(OwnerCharacter->GetMovementComponent());
	}

}

void UALSComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (OwnerCharacter)
	{
		OwnerCharacter->MovementModeChangedDelegate.RemoveDynamic(this,&ThisClass::OnMovementModeChanged);
	}
	Super::EndPlay(EndPlayReason);
}


UALSComponent* UALSComponent::FindALSComponent(const AActor* Actor)
{
	return Actor != nullptr ? Actor->FindComponentByClass<UALSComponent>():nullptr;
}

void UALSComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Set required values
	SetEssentialValues(DeltaTime);

	if (MovementState == FALSGameplayTags::Get().MovementState_Grounded)
	{
		UpdateCharacterMovement();
		UpdateGroundedRotation(DeltaTime);
	}
	else if (MovementState == FALSGameplayTags::Get().MovementState_InAir)
	{
		UpdateInAirRotation(DeltaTime);
	}
	else if (MovementState == FALSGameplayTags::Get().MovementState_Ragdoll)
	{
		RagdollUpdate(DeltaTime);
	}

	// Cache values
	PreviousVelocity = OwnerCharacter->GetVelocity();
	PreviousAimYaw = AimingRotation.Yaw;
}

FName UALSComponent::GetRagdollBoneName() const
{
	return UALSSettings::Get()->NAME_Pelvis;
}

void UALSComponent::RagdollStart()
{
	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(true);
	}

	/** When Networked, disables replicate movement reset TargetRagdollLocation and ServerRagdollPull variable
	and if the host is a dedicated server, change character mesh optimisation option to avoid z-location bug*/
	OwnerCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = 1;

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		DefVisBasedTickOp = OwnerCharacter->GetMesh()->VisibilityBasedAnimTickOption;
		OwnerCharacter->GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	}
	TargetRagdollLocation = OwnerCharacter->GetMesh()->GetSocketLocation(GetRagdollBoneName());
	ServerRagdollPull = 0;

	// Disable URO
	bPreRagdollURO = OwnerCharacter->GetMesh()->bEnableUpdateRateOptimizations;
	OwnerCharacter->GetMesh()->bEnableUpdateRateOptimizations = false;

	// Step 1: Clear the Character Movement Mode and set the Movement State to Ragdoll
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
	SetMovementState(FALSGameplayTags::Get().MovementState_Ragdoll);

	// Step 2: Disable capsule collision and enable mesh physics simulation starting from the pelvis.
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PrevCollisionObjectType = OwnerCharacter->GetMesh()->GetCollisionObjectType();
	OwnerCharacter->GetMesh()->SetCollisionObjectType(ECC_PhysicsBody);
	OwnerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwnerCharacter->GetMesh()->SetAllBodiesBelowSimulatePhysics(GetRagdollBoneName(), true, true);

	// Step 3: Stop any active montages.
	if (OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Stop(0.2f);
	}

	// Fixes character mesh is showing default A pose for a split-second just before ragdoll ends in listen server games
	OwnerCharacter->GetMesh()->bOnlyAllowAutonomousTickPose = true;

	OwnerCharacter->SetReplicateMovement(false);
}

void UALSComponent::RagdollEnd()
{
	/** Re-enable Replicate Movement and if the host is a dedicated server set mesh visibility based anim
	tick option back to default*/

	if (UKismetSystemLibrary::IsDedicatedServer(GetWorld()))
	{
		OwnerCharacter->GetMesh()->VisibilityBasedAnimTickOption = DefVisBasedTickOp;
	}

	OwnerCharacter->GetMesh()->bEnableUpdateRateOptimizations = bPreRagdollURO;

	// Revert back to default settings
	OwnerCharacter->GetCharacterMovement()->bIgnoreClientMovementErrorChecksAndCorrection = 0;
	OwnerCharacter->GetMesh()->bOnlyAllowAutonomousTickPose = false;
	OwnerCharacter->SetReplicateMovement(true);

	// Step 1: Save a snapshot of the current Ragdoll Pose for use in AnimGraph to blend out of the ragdoll
	if (OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->SavePoseSnapshot(UALSSettings::Get()->NAME_RagdollPose);
	}

	// Step 2: If the ragdoll is on the ground, set the movement mode to walking and play a Get Up animation.
	// If not, set the movement mode to falling and update the character movement velocity to match the last ragdoll velocity.
	if (bRagdollOnGround)
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		if (OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(GetGetUpAnimation(bRagdollFaceUp), 1.0f, EMontagePlayReturnType::MontageLength, 0.0f, true);
		}
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		OwnerCharacter->GetCharacterMovement()->Velocity = LastRagdollVelocity;
	}

	// Step 3: Re-Enable capsule collision, and disable physics simulation on the mesh.
	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwnerCharacter->GetMesh()->SetCollisionObjectType(PrevCollisionObjectType);
	OwnerCharacter->GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OwnerCharacter->GetMesh()->SetAllBodiesSimulatePhysics(false);

	if (RagdollStateChangedDelegate.IsBound())
	{
		RagdollStateChangedDelegate.Broadcast(false);
	}
}

void UALSComponent::Server_SetMeshLocationDuringRagdoll_Implementation(FVector MeshLocation)
{
	TargetRagdollLocation = MeshLocation;
}

void UALSComponent::SetMovementState(const FGameplayTag NewState, bool bForce)
{
	if (bForce || MovementState != NewState)
	{
		PrevMovementState = MovementState;
		MovementState = NewState;
		OnMovementStateChanged(PrevMovementState);
		OnMovementStateChangedEvent.Broadcast(PrevMovementState);
	}
}

void UALSComponent::SetMovementAction(const FGameplayTag NewAction, bool bForce)
{
	if (bForce || MovementAction != NewAction)
	{
		const FGameplayTag Prev = MovementAction;
		MovementAction = NewAction;
		OnMovementActionChanged(Prev);
		OnMovementActionChangedEvent.Broadcast(Prev);
	}
}

void UALSComponent::SetStance(const FGameplayTag NewStance, bool bForce)
{
	if (bForce || Stance != NewStance)
	{
		const FGameplayTag Prev = Stance;
		Stance = NewStance;
		OnStanceChanged(Prev);
		OnStanceChangedEvent.Broadcast(Prev);
	}
}

void UALSComponent::SetOverlayOverrideState(int32 NewState)
{
	OverlayOverrideState = NewState;
}

void UALSComponent::SetGait(const FGameplayTag NewGait, bool bForce)
{
	if (bForce || Gait != NewGait)
	{
		const FGameplayTag Prev = Gait;
		Gait = NewGait;
		OnGaitChanged(Prev);
		OnGaitChangedEvent.Broadcast(Prev);
	}
}


void UALSComponent::SetDesiredStance(FGameplayTag NewStance)
{
	DesiredStance = NewStance;
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredStance(NewStance);
	}
}

void UALSComponent::Server_SetDesiredStance_Implementation(FGameplayTag NewStance)
{
	SetDesiredStance(NewStance);
}

void UALSComponent::SetDesiredGait(const FGameplayTag NewGait)
{
	DesiredGait = NewGait;
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredGait(NewGait);
	}
}

void UALSComponent::Server_SetDesiredGait_Implementation(FGameplayTag NewGait)
{
	SetDesiredGait(NewGait);
}

void UALSComponent::SetDesiredRotationMode(FGameplayTag NewRotMode)
{
	DesiredRotationMode = NewRotMode;
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		Server_SetDesiredRotationMode(NewRotMode);
	}
}

void UALSComponent::Server_SetDesiredRotationMode_Implementation(FGameplayTag NewRotMode)
{
	SetDesiredRotationMode(NewRotMode);
}

void UALSComponent::SetRotationMode(const FGameplayTag NewRotationMode, bool bForce)
{
	if (bForce || RotationMode != NewRotationMode)
	{
		const FGameplayTag Prev = RotationMode;
		RotationMode = NewRotationMode;
		OnRotationModeChanged(Prev);
		OnRotationModeChangedEvent.Broadcast(RotationMode);

		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			Server_SetRotationMode(NewRotationMode, bForce);
		}
	}
}


void UALSComponent::Server_SetRotationMode_Implementation(FGameplayTag NewRotationMode, bool bForce)
{
	SetRotationMode(NewRotationMode, bForce);
}

void UALSComponent::SetViewMode(const FGameplayTag NewViewMode, bool bForce)
{
	if (bForce || ViewMode != NewViewMode)
	{
		const FGameplayTag Prev = ViewMode;
		ViewMode = NewViewMode;
		OnViewModeChanged(Prev);
		OnViewModeChangedEvent.Broadcast(Prev);

		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			Server_SetViewMode(NewViewMode, bForce);
		}
	}
}

void UALSComponent::Server_SetViewMode_Implementation(FGameplayTag NewViewMode, bool bForce)
{
	SetViewMode(NewViewMode, bForce);
}

void UALSComponent::SetOverlayState(const FGameplayTag NewState, bool bForce)
{
	if (bForce || OverlayState != NewState)
	{
		const FGameplayTag Prev = OverlayState;
		OverlayState = NewState;
		OnOverlayStateChanged(Prev);
		OnOverlayStateChangedEvent.Broadcast(Prev);

		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			Server_SetOverlayState(NewState, bForce);
		}
	}
}

void UALSComponent::SetGroundedEntryState(FGameplayTag NewState)
{
	GroundedEntryState = NewState;
}


void UALSComponent::Server_SetOverlayState_Implementation(FGameplayTag NewState, bool bForce)
{
	SetOverlayState(NewState, bForce);
}

void UALSComponent::EventOnLanded()
{
	const float VelZ = FMath::Abs(OwnerCharacter->GetCharacterMovement()->Velocity.Z);

	if (bRagdollOnLand && VelZ > RagdollOnLandVelocity)
	{
		ReplicatedRagdollStart();
	}
	else if (bBreakfallOnLand && bHasMovementInput && VelZ >= BreakfallOnLandVelocity)
	{
		OnBreakfall();
	}
	else
	{
		OwnerCharacter->GetCharacterMovement()->BrakingFrictionFactor = bHasMovementInput ? 0.5f : 3.0f;

		// After 0.5 secs, reset braking friction factor to zero
		OwnerCharacter->GetWorldTimerManager().SetTimer(OnLandedFrictionResetTimer, this,
		                                &UALSComponent::OnLandFrictionReset, 0.5f, false);
	}
}

void UALSComponent::Multicast_OnLanded_Implementation()
{
	if (!OwnerCharacter->IsLocallyControlled())
	{
		EventOnLanded();
	}
}

void UALSComponent::EventOnJumped()
{
	// Set the new In Air Rotation to the velocity rotation if speed is greater than 100.
	InAirRotation = Speed > 100.0f ? LastVelocityRotation : OwnerCharacter->GetActorRotation();

	OnJumpedDelegate.Broadcast();
}

void UALSComponent::Server_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}

	OwnerCharacter->ForceNetUpdate();
	Multicast_PlayMontage(Montage, PlayRate);
}

void UALSComponent::Multicast_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate)
{
	if (OwnerCharacter->GetMesh()->GetAnimInstance() && !OwnerCharacter->IsLocallyControlled())
	{
		OwnerCharacter->GetMesh()->GetAnimInstance()->Montage_Play(Montage, PlayRate);
	}
}

void UALSComponent::Multicast_OnJumped_Implementation()
{
	if (!OwnerCharacter->IsLocallyControlled())
	{
		EventOnJumped();
	}
}

void UALSComponent::Server_RagdollStart_Implementation()
{
	Multicast_RagdollStart();
}

void UALSComponent::Multicast_RagdollStart_Implementation()
{
	RagdollStart();
}

void UALSComponent::Server_RagdollEnd_Implementation(FVector CharacterLocation)
{
	Multicast_RagdollEnd(CharacterLocation);
}

void UALSComponent::Multicast_RagdollEnd_Implementation(FVector CharacterLocation)
{
	RagdollEnd();
}

void UALSComponent::SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation)
{
	OwnerCharacter->SetActorLocationAndRotation(NewLocation, NewRotation);
	TargetRotation = NewRotation;
}

void UALSComponent::SetMovementModel(const FDataTableRowHandle& InModel)
{
	const FString ContextString = GetFullName();
	FALSMovementStateSettings* OutRow =
		InModel.DataTable->FindRow<FALSMovementStateSettings>(InModel.RowName, ContextString);
	check(OutRow);
	MovementData = *OutRow;
}

void UALSComponent::ForceUpdateCharacterState()
{
	SetGait(DesiredGait, true);
	SetStance(DesiredStance, true);
	SetRotationMode(DesiredRotationMode, true);
	SetViewMode(ViewMode, true);
	SetOverlayState(OverlayState, true);
	SetMovementState(MovementState, true);
	SetMovementAction(MovementAction, true);
}

FALSMovementSettings UALSComponent::GetTargetMovementSettings() const
{
	if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection)
	{
		if (Stance == FALSGameplayTags::Get().Stance_Standing)
		{
			return MovementData.VelocityDirection.Standing;
		}
		if (Stance == FALSGameplayTags::Get().Stance_Crouching)
		{
			return MovementData.VelocityDirection.Crouching;
		}
	}
	else if (RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection)
	{
		if (Stance == FALSGameplayTags::Get().Stance_Standing)
		{
			return MovementData.LookingDirection.Standing;
		}
		if (Stance == FALSGameplayTags::Get().Stance_Crouching)
		{
			return MovementData.LookingDirection.Crouching;
		}
	}
	else if (RotationMode == FALSGameplayTags::Get().RotationMode_Aiming)
	{
		if (Stance == FALSGameplayTags::Get().Stance_Standing)
		{
			return MovementData.Aiming.Standing;
		}
		if (Stance == FALSGameplayTags::Get().Stance_Crouching)
		{
			return MovementData.Aiming.Crouching;
		}
	}

	// Default to velocity dir standing
	return MovementData.VelocityDirection.Standing;
}

bool UALSComponent::CanSprint() const
{
	// Determine if the character is currently able to sprint based on the Rotation mode and current acceleration
	// (input) rotation. If the character is in the Looking Rotation mode, only allow sprinting if there is full
	// movement input and it is faced forward relative to the camera + or - 50 degrees.

	if (!bHasMovementInput || RotationMode == FALSGameplayTags::Get().RotationMode_Aiming)
	{
		return false;
	}

	const bool bValidInputAmount = MovementInputAmount > 0.9f;

	if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection)
	{
		return bValidInputAmount;
	}

	if (RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection)
	{
		const FRotator AccRot = ReplicatedCurrentAcceleration.ToOrientationRotator();
		FRotator Delta = AccRot - AimingRotation;
		Delta.Normalize();

		return bValidInputAmount && FMath::Abs(Delta.Yaw) < 50.0f;
	}

	return false;
}

FVector UALSComponent::GetMovementInput() const
{
	return ReplicatedCurrentAcceleration;
}

float UALSComponent::GetAnimCurveValue(FName CurveName) const
{
	if (OwnerCharacter->GetMesh()->GetAnimInstance())
	{
		return OwnerCharacter->GetMesh()->GetAnimInstance()->GetCurveValue(CurveName);
	}

	return 0.0f;
}

void UALSComponent::SetVisibleMesh(USkeletalMesh* NewVisibleMesh)
{
	if (VisibleMesh != NewVisibleMesh)
	{
		const USkeletalMesh* Prev = VisibleMesh;
		VisibleMesh = NewVisibleMesh;
		OnVisibleMeshChanged(Prev);

		if (GetOwnerRole() != ROLE_Authority)
		{
			Server_SetVisibleMesh(NewVisibleMesh);
		}
	}
}

void UALSComponent::Server_SetVisibleMesh_Implementation(USkeletalMesh* NewVisibleMesh)
{
	SetVisibleMesh(NewVisibleMesh);
}

void UALSComponent::SetRightShoulder(bool bNewRightShoulder)
{
	if (bRightShoulder != bNewRightShoulder)
	{
		bool bPrev = bRightShoulder;
		bRightShoulder = bNewRightShoulder;
		OnShoulderChangedEvent.Broadcast(bPrev);
	}

}

ECollisionChannel UALSComponent::GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius)
{
	const FName CameraSocketName = bRightShoulder ? TEXT("TP_CameraTrace_R") : TEXT("TP_CameraTrace_L");
	TraceOrigin = OwnerCharacter->GetMesh()->GetSocketLocation(CameraSocketName);
	TraceRadius = 15.0f;
	return ECC_Camera;
	// TraceOrigin = OwnerCharacter->GetActorLocation();
	// TraceRadius = 10.0f;
	// return ECC_Visibility;
}

FTransform UALSComponent::GetThirdPersonPivotTarget()
{
	// GetOwner()->GetActorTransform();
	USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
	return FTransform(GetOwner()->GetActorRotation(),
					  (OwnerMesh->GetSocketLocation(TEXT("Head")) + OwnerMesh->GetSocketLocation(TEXT("root"))) / 2.0f,
					  FVector::OneVector);
}

FVector UALSComponent::GetFirstPersonCameraTarget()
{
	return OwnerCharacter->GetMesh()->GetSocketLocation(UALSSettings::Get()->NAME_FP_Camera);
}

void UALSComponent::GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const
{
	TPFOVOut = ThirdPersonFOV;
	FPFOVOut = FirstPersonFOV;
	bRightShoulderOut = bRightShoulder;
}

void UALSComponent::RagdollUpdate(float DeltaTime)
{
	OwnerCharacter->GetMesh()->bOnlyAllowAutonomousTickPose = false;

	// Set the Last Ragdoll Velocity.
	const FVector NewRagdollVel = OwnerCharacter->GetMesh()->GetPhysicsLinearVelocity(UALSSettings::Get()->NAME_root);
	LastRagdollVelocity = (NewRagdollVel != FVector::ZeroVector || OwnerCharacter->IsLocallyControlled())
		                      ? NewRagdollVel
		                      : LastRagdollVelocity / 2;

	// Use the Ragdoll Velocity to scale the ragdoll's joint strength for physical animation.
	const float SpringValue = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 1000.0f}, {0.0f, 25000.0f},
	                                                            LastRagdollVelocity.Size());
	OwnerCharacter->GetMesh()->SetAllMotorsAngularDriveParams(SpringValue, 0.0f, 0.0f, false);

	// Disable Gravity if falling faster than -4000 to prevent continual acceleration.
	// This also prevents the ragdoll from going through the floor.
	const bool bEnableGrav = LastRagdollVelocity.Z > -4000.0f;
	OwnerCharacter->GetMesh()->SetEnableGravity(bEnableGrav);

	// Update the Actor location to follow the ragdoll.
	SetActorLocationDuringRagdoll(DeltaTime);
}

void UALSComponent::SetActorLocationDuringRagdoll(float DeltaTime)
{
	if (OwnerCharacter->IsLocallyControlled())
	{
		// Set the pelvis as the target location.
		TargetRagdollLocation = OwnerCharacter->GetMesh()->GetSocketLocation(GetRagdollBoneName());
		if (!OwnerCharacter->HasAuthority())
		{
			Server_SetMeshLocationDuringRagdoll(TargetRagdollLocation);
		}
	}

	// Determine whether the ragdoll is facing up or down and set the target rotation accordingly.
	const FRotator PelvisRot = OwnerCharacter->GetMesh()->GetSocketRotation(GetRagdollBoneName());

	if (bReversedPelvis) {
		bRagdollFaceUp = PelvisRot.Roll > 0.0f;
	} else
	{
		bRagdollFaceUp = PelvisRot.Roll < 0.0f;
	}


	const FRotator TargetRagdollRotation(0.0f, bRagdollFaceUp ? PelvisRot.Yaw - 180.0f : PelvisRot.Yaw, 0.0f);

	// Trace downward from the target location to offset the target location,
	// preventing the lower half of the capsule from going through the floor when the ragdoll is laying on the ground.
	const FVector TraceVect(TargetRagdollLocation.X, TargetRagdollLocation.Y,
	                        TargetRagdollLocation.Z - OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	UWorld* World = GetWorld();
	check(World);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	FHitResult HitResult;
	const bool bHit = World->LineTraceSingleByChannel(HitResult, TargetRagdollLocation, TraceVect,
	                                                  ECC_Visibility, Params);

	if (ALSDebugComponent && ALSDebugComponent->GetShowTraces())
	{
		UALSDebugComponent::DrawDebugLineTraceSingle(World,
		                                             TargetRagdollLocation,
		                                             TraceVect,
		                                             EDrawDebugTrace::Type::ForOneFrame,
		                                             bHit,
		                                             HitResult,
		                                             FLinearColor::Red,
		                                             FLinearColor::Green,
		                                             1.0f);
	}

	bRagdollOnGround = HitResult.IsValidBlockingHit();
	FVector NewRagdollLoc = TargetRagdollLocation;

	if (bRagdollOnGround)
	{
		const float ImpactDistZ = FMath::Abs(HitResult.ImpactPoint.Z - HitResult.TraceStart.Z);
		NewRagdollLoc.Z += OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - ImpactDistZ + 2.0f;
	}
	if (!OwnerCharacter->IsLocallyControlled())
	{
		ServerRagdollPull = FMath::FInterpTo(ServerRagdollPull, 750.0f, DeltaTime, 0.6f);
		float RagdollSpeed = FVector(LastRagdollVelocity.X, LastRagdollVelocity.Y, 0).Size();
		FName RagdollSocketPullName = RagdollSpeed > 300 ? UALSSettings::Get()->NAME_spine_03 : GetRagdollBoneName();
		OwnerCharacter->GetMesh()->AddForce(
			(TargetRagdollLocation - OwnerCharacter->GetMesh()->GetSocketLocation(RagdollSocketPullName)) * ServerRagdollPull,
			RagdollSocketPullName, true);
	}
	SetActorLocationAndTargetRotation(bRagdollOnGround ? NewRagdollLoc : TargetRagdollLocation, TargetRagdollRotation);
}

void UALSComponent::OnMovementModeChanged(ACharacter* InCharacter,EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	// Use the Character Movement Mode changes to set the Movement States to the right values. This allows you to have
	// a custom set of movement states but still use the functionality of the default character movement component.

	if (OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_Walking ||
		OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_NavWalking)
	{
		SetMovementState(FALSGameplayTags::Get().MovementState_Grounded);
	}
	else if (OwnerCharacter->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		SetMovementState(FALSGameplayTags::Get().MovementState_InAir);
	}
}

void UALSComponent::OnMovementStateChanged(const FGameplayTag PreviousState)
{
	if (MovementState == FALSGameplayTags::Get().MovementState_InAir)
	{
		if (MovementAction == FALSGameplayTags::Get().MovementAction_None)
		{
			// If the character enters the air, set the In Air Rotation and uncrouch if crouched.
			InAirRotation = OwnerCharacter->GetActorRotation();
			if (Stance == FALSGameplayTags::Get().Stance_Crouching)
			{
				OwnerCharacter->UnCrouch();
			}
		}
		else if (MovementAction == FALSGameplayTags::Get().MovementAction_Rolling)
		{
			// If the character is currently rolling, enable the ragdoll.
			ReplicatedRagdollStart();
		}
	}
}

void UALSComponent::OnMovementActionChanged(const FGameplayTag PreviousAction)
{
	// Make the character crouch if performing a roll.
	if (MovementAction == FALSGameplayTags::Get().MovementAction_Rolling)
	{
		OwnerCharacter->Crouch();
	}

	if (PreviousAction == FALSGameplayTags::Get().MovementAction_Rolling)
	{
		if (DesiredStance == FALSGameplayTags::Get().Stance_Standing)
		{
			OwnerCharacter->UnCrouch();
		}
		else if (DesiredStance == FALSGameplayTags::Get().Stance_Crouching)
		{
			OwnerCharacter->Crouch();
		}
	}
}

void UALSComponent::OnStanceChanged(const FGameplayTag PreviousStance)
{
	if (MyCharacterMovementComponent)
	{
		MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

void UALSComponent::OnRotationModeChanged(FGameplayTag PreviousRotationMode)
{
	if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection && ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson)
	{
		// If the new rotation mode is Velocity Direction and the character is in First Person,
		// set the viewmode to Third Person.
		SetViewMode(FALSGameplayTags::Get().ViewMode_ThirdPerson);
	}
	if (MyCharacterMovementComponent)
	{
		MyCharacterMovementComponent->SetMovementSettings(GetTargetMovementSettings());
	}
}

void UALSComponent::OnGaitChanged(const FGameplayTag PreviousGait)
{
}

void UALSComponent::OnViewModeChanged(const FGameplayTag PreviousViewMode)
{
	if (ViewMode == FALSGameplayTags::Get().ViewMode_ThirdPerson)
	{
		if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection || RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection)
		{
			// If Third Person, set the rotation mode back to the desired mode.
			SetRotationMode(DesiredRotationMode);
		}
	}
	else if (ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson && RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection)
	{
		// If First Person, set the rotation mode to looking direction if currently in the velocity direction mode.
		SetRotationMode(FALSGameplayTags::Get().RotationMode_LookingDirection);
	}
}

void UALSComponent::OnOverlayStateChanged(const FGameplayTag PreviousState)
{
}

void UALSComponent::OnVisibleMeshChanged(const USkeletalMesh* PrevVisibleMesh)
{
	// Update the Skeletal Mesh before we update materials and anim bp variables
	OwnerCharacter->GetMesh()->SetSkeletalMesh(VisibleMesh);

	// Reset materials to their new mesh defaults
	if (OwnerCharacter->GetMesh() != nullptr)
	{
		for (int32 MaterialIndex = 0; MaterialIndex < OwnerCharacter->GetMesh()->GetNumMaterials(); ++MaterialIndex)
		{
			OwnerCharacter->GetMesh()->SetMaterial(MaterialIndex, nullptr);
		}
	}

	// Force set variables. This ensures anim instance & character stay synchronized on mesh changes
	ForceUpdateCharacterState();
}

void UALSComponent::OnOwnerCharacterStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	// Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(FALSGameplayTags::Get().Stance_Crouching);
}

void UALSComponent::OnOwnerCharacterEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	// Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	SetStance(FALSGameplayTags::Get().Stance_Standing);
}

void UALSComponent::OnOwnerCharacterJumped()
{
	// Super::OnJumped_Implementation();
	if (OwnerCharacter->IsLocallyControlled())
	{
		EventOnJumped();
	}
	if (OwnerCharacter->HasAuthority())
	{
		Multicast_OnJumped();
	}
}

void UALSComponent::OnOwnerCharacterLanded(const FHitResult& Hit)
{
	// Super::Landed(Hit);

	if (OwnerCharacter->IsLocallyControlled())
	{
		EventOnLanded();
	}
	if (OwnerCharacter->HasAuthority())
	{
		Multicast_OnLanded();
	}
}

void UALSComponent::OnLandFrictionReset()
{
	// Reset the braking friction
	OwnerCharacter->GetCharacterMovement()->BrakingFrictionFactor = 0.0f;
}

void UALSComponent::SetEssentialValues(float DeltaTime)
{
	if (GetOwnerRole() != ROLE_SimulatedProxy)
	{
		ReplicatedCurrentAcceleration = OwnerCharacter->GetCharacterMovement()->GetCurrentAcceleration();
		ReplicatedControlRotation = OwnerCharacter->GetControlRotation();
		EasedMaxAcceleration = OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration();
	}
	else
	{
		EasedMaxAcceleration = OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration() != 0
			                       ? OwnerCharacter->GetCharacterMovement()->GetMaxAcceleration()
			                       : EasedMaxAcceleration / 2;
	}

	// Interp AimingRotation to current control rotation for smooth character rotation movement. Decrease InterpSpeed
	// for slower but smoother movement.
	AimingRotation = FMath::RInterpTo(AimingRotation, ReplicatedControlRotation, DeltaTime, 30);

	// These values represent how the capsule is moving as well as how it wants to move, and therefore are essential
	// for any data driven animation system. They are also used throughout the system for various functions,
	// so I found it is easiest to manage them all in one place.

	const FVector CurrentVel = OwnerCharacter->GetVelocity();

	// Set the amount of Acceleration.
	const FVector NewAcceleration = (CurrentVel - PreviousVelocity) / DeltaTime;
	Acceleration = NewAcceleration.IsNearlyZero() || OwnerCharacter->IsLocallyControlled() ? NewAcceleration : Acceleration / 2;

	// Determine if the character is moving by getting it's speed. The Speed equals the length of the horizontal (x y)
	// velocity, so it does not take vertical movement into account. If the character is moving, update the last
	// velocity rotation. This value is saved because it might be useful to know the last orientation of movement
	// even after the character has stopped.
	Speed = CurrentVel.Size2D();
	bIsMoving = Speed > 1.0f;
	if (bIsMoving)
	{
		LastVelocityRotation = CurrentVel.ToOrientationRotator();
	}

	// Determine if the character has movement input by getting its movement input amount.
	// The Movement Input Amount is equal to the current acceleration divided by the max acceleration so that
	// it has a range of 0-1, 1 being the maximum possible amount of input, and 0 being none.
	// If the character has movement input, update the Last Movement Input Rotation.
	MovementInputAmount = ReplicatedCurrentAcceleration.Size() / EasedMaxAcceleration;
	bHasMovementInput = MovementInputAmount > 0.0f;
	if (bHasMovementInput)
	{
		LastMovementInputRotation = ReplicatedCurrentAcceleration.ToOrientationRotator();
	}

	// Set the Aim Yaw rate by comparing the current and previous Aim Yaw value, divided by Delta Seconds.
	// This represents the speed the camera is rotating left to right.
	AimYawRate = FMath::Abs((AimingRotation.Yaw - PreviousAimYaw) / DeltaTime);
}

void UALSComponent::UpdateCharacterMovement()
{
	// Set the Allowed Gait
	const FGameplayTag AllowedGait = GetAllowedGait();

	// Determine the Actual Gait. If it is different from the current Gait, Set the new Gait Event.
	const FGameplayTag ActualGait = GetActualGait(AllowedGait);

	if (ActualGait != Gait)
	{
		SetGait(ActualGait);
	}

	// Update the Character Max Walk Speed to the configured speeds based on the currently Allowed Gait.
	if (MyCharacterMovementComponent)
	{
		MyCharacterMovementComponent->SetAllowedGait(AllowedGait);
	}
}

void UALSComponent::UpdateGroundedRotation(float DeltaTime)
{
	if (MovementAction == FALSGameplayTags::Get().MovementAction_None)
	{
		const bool bCanUpdateMovingRot = ((bIsMoving && bHasMovementInput) || Speed > 150.0f) && !OwnerCharacter->HasAnyRootMotion();
		if (bCanUpdateMovingRot)
		{
			const float GroundedRotationRate = CalculateGroundedRotationRate();
			if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection)
			{
				// Velocity Direction Rotation
				SmoothCharacterRotation({0.0f, LastVelocityRotation.Yaw, 0.0f}, 800.0f, GroundedRotationRate,
				                        DeltaTime);
			}
			else if (RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection)
			{
				// Looking Direction Rotation
				float YawValue;
				if (Gait == FALSGameplayTags::Get().Gait_Sprinting)
				{
					YawValue = LastVelocityRotation.Yaw;
				}
				else
				{
					// Walking or Running..
					const float YawOffsetCurveVal = GetAnimCurveValue(UALSSettings::Get()->NAME_YawOffset);
					YawValue = AimingRotation.Yaw + YawOffsetCurveVal;
				}
				SmoothCharacterRotation({0.0f, YawValue, 0.0f}, 500.0f, GroundedRotationRate, DeltaTime);
			}
			else if (RotationMode == FALSGameplayTags::Get().RotationMode_Aiming)
			{
				const float ControlYaw = AimingRotation.Yaw;
				SmoothCharacterRotation({0.0f, ControlYaw, 0.0f}, 1000.0f, 20.0f, DeltaTime);
			}
		}
		else
		{
			// Not Moving

			if ((ViewMode == FALSGameplayTags::Get().ViewMode_ThirdPerson && RotationMode == FALSGameplayTags::Get().RotationMode_Aiming) ||
				ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson)
			{
				LimitRotation(-100.0f, 100.0f, 20.0f, DeltaTime);
			}

			// Apply the RotationAmount curve from Turn In Place Animations.
			// The Rotation Amount curve defines how much rotation should be applied each frame,
			// and is calculated for animations that are animated at 30fps.

			const float RotAmountCurve = GetAnimCurveValue(UALSSettings::Get()->NAME_RotationAmount);

			if (FMath::Abs(RotAmountCurve) > 0.001f)
			{
				if (GetOwnerRole() == ROLE_AutonomousProxy)
				{
					TargetRotation.Yaw = UKismetMathLibrary::NormalizeAxis(
						TargetRotation.Yaw + (RotAmountCurve * (DeltaTime / (1.0f / 30.0f))));
					OwnerCharacter->SetActorRotation(TargetRotation);
				}
				else
				{
					OwnerCharacter->AddActorWorldRotation({0, RotAmountCurve * (DeltaTime / (1.0f / 30.0f)), 0});
				}
				TargetRotation = OwnerCharacter->GetActorRotation();
			}
		}
	}
	else if (MovementAction == FALSGameplayTags::Get().MovementAction_Rolling)
	{
		// Rolling Rotation (Not allowed on networked games)
		if (!bEnableNetworkOptimizations && bHasMovementInput)
		{
			SmoothCharacterRotation({0.0f, LastMovementInputRotation.Yaw, 0.0f}, 0.0f, 2.0f, DeltaTime);
		}
	}

	// Other actions are ignored...
}

void UALSComponent::UpdateInAirRotation(float DeltaTime)
{
	if (RotationMode == FALSGameplayTags::Get().RotationMode_VelocityDirection || RotationMode == FALSGameplayTags::Get().RotationMode_LookingDirection)
	{
		// Velocity / Looking Direction Rotation
		SmoothCharacterRotation({0.0f, InAirRotation.Yaw, 0.0f}, 0.0f, 5.0f, DeltaTime);
	}
	else if (RotationMode == FALSGameplayTags::Get().RotationMode_Aiming)
	{
		// Aiming Rotation
		SmoothCharacterRotation({0.0f, AimingRotation.Yaw, 0.0f}, 0.0f, 15.0f, DeltaTime);
		InAirRotation = OwnerCharacter->GetActorRotation();
	}
}

FGameplayTag UALSComponent::GetAllowedGait() const
{
	// Calculate the Allowed Gait. This represents the maximum Gait the character is currently allowed to be in,
	// and can be determined by the desired gait, the rotation mode, the stance, etc. For example,
	// if you wanted to force the character into a walking state while indoors, this could be done here.

	if (Stance == FALSGameplayTags::Get().Stance_Standing)
	{
		if (RotationMode != FALSGameplayTags::Get().RotationMode_Aiming)
		{
			if (DesiredGait == FALSGameplayTags::Get().Gait_Sprinting)
			{
				return CanSprint() ? FALSGameplayTags::Get().Gait_Sprinting : FALSGameplayTags::Get().Gait_Running;
			}
			return DesiredGait;
		}
	}

	// Crouching stance & Aiming rot mode has same behaviour

	if (DesiredGait == FALSGameplayTags::Get().Gait_Sprinting)
	{
		return FALSGameplayTags::Get().Gait_Running;
	}

	return DesiredGait;
}

FGameplayTag UALSComponent::GetActualGait(FGameplayTag AllowedGait) const
{
	// Get the Actual Gait. This is calculated by the actual movement of the character,  and so it can be different
	// from the desired gait or allowed gait. For instance, if the Allowed Gait becomes walking,
	// the Actual gait will still be running until the character decelerates to the walking speed.
	if (!MyCharacterMovementComponent)
	{
		return FALSGameplayTags::Get().Gait_Walking;
	}
	const float LocWalkSpeed = MyCharacterMovementComponent->CurrentMovementSettings.WalkSpeed;
	const float LocRunSpeed = MyCharacterMovementComponent->CurrentMovementSettings.RunSpeed;

	if (Speed > LocRunSpeed + 10.0f)
	{
		if (AllowedGait == FALSGameplayTags::Get().Gait_Sprinting)
		{
			return FALSGameplayTags::Get().Gait_Sprinting;
		}
		return FALSGameplayTags::Get().Gait_Running;
	}

	if (Speed >= LocWalkSpeed + 10.0f)
	{
		return FALSGameplayTags::Get().Gait_Running;
	}

	return FALSGameplayTags::Get().Gait_Walking;
}

void UALSComponent::SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed,
                                                float DeltaTime)
{
	// Interpolate the Target Rotation for extra smooth rotation behavior
	TargetRotation =
		FMath::RInterpConstantTo(TargetRotation, Target, DeltaTime, TargetInterpSpeed);
	OwnerCharacter->SetActorRotation(
		FMath::RInterpTo(OwnerCharacter->GetActorRotation(), TargetRotation, DeltaTime, ActorInterpSpeed));
}

float UALSComponent::CalculateGroundedRotationRate() const
{
	// Calculate the rotation rate by using the current Rotation Rate Curve in the Movement Settings.
	// Using the curve in conjunction with the mapped speed gives you a high level of control over the rotation
	// rates for each speed. Increase the speed if the camera is rotating quickly for more responsive rotation.

	const float MappedSpeedVal = MyCharacterMovementComponent->GetMappedSpeed();
	const float CurveVal =
		MyCharacterMovementComponent->CurrentMovementSettings.RotationRateCurve->GetFloatValue(MappedSpeedVal);
	const float ClampedAimYawRate = FMath::GetMappedRangeValueClamped<float, float>({0.0f, 300.0f}, {1.0f, 3.0f}, AimYawRate);
	return CurveVal * ClampedAimYawRate;
}

void UALSComponent::LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime)
{
	// Prevent the character from rotating past a certain angle.
	FRotator Delta = AimingRotation - OwnerCharacter->GetActorRotation();
	Delta.Normalize();
	const float RangeVal = Delta.Yaw;

	if (RangeVal < AimYawMin || RangeVal > AimYawMax)
	{
		const float ControlRotYaw = AimingRotation.Yaw;
		const float TargetYaw = ControlRotYaw + (RangeVal > 0.0f ? AimYawMin : AimYawMax);
		SmoothCharacterRotation({0.0f, TargetYaw, 0.0f}, 0.0f, InterpSpeed, DeltaTime);
	}
}

void UALSComponent::ForwardMovementAction_Implementation(float Value)
{
	if (MovementState == FALSGameplayTags::Get().MovementState_Grounded || MovementState == FALSGameplayTags::Get().MovementState_InAir)
	{
		// Default camera relative movement behavior
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		OwnerCharacter->AddMovementInput(UKismetMathLibrary::GetForwardVector(DirRotator), Value);
	}
}

void UALSComponent::RightMovementAction_Implementation(float Value)
{
	if (MovementState == FALSGameplayTags::Get().MovementState_Grounded || MovementState == FALSGameplayTags::Get().MovementState_InAir)
	{
		// Default camera relative movement behavior
		const FRotator DirRotator(0.0f, AimingRotation.Yaw, 0.0f);
		OwnerCharacter->AddMovementInput(UKismetMathLibrary::GetRightVector(DirRotator), Value);
	}
}

void UALSComponent::CameraUpAction_Implementation(float Value)
{
	OwnerCharacter->AddControllerPitchInput(LookUpDownRate * Value);
}

void UALSComponent::CameraRightAction_Implementation(float Value)
{
	OwnerCharacter->AddControllerYawInput(LookLeftRightRate * Value);
}

void UALSComponent::JumpAction_Implementation(bool bValue)
{
	if (bValue)
	{
		// Jump Action: Press "Jump Action" to end the ragdoll if ragdolling, stand up if crouching, or jump if standing.
		if (JumpPressedDelegate.IsBound())
		{
			JumpPressedDelegate.Broadcast();
		}

		if (MovementAction == FALSGameplayTags::Get().MovementAction_None)
		{
			if (MovementState == FALSGameplayTags::Get().MovementState_Grounded)
			{
				if (Stance == FALSGameplayTags::Get().Stance_Standing)
				{
					OwnerCharacter->Jump();
				}
				else if (Stance == FALSGameplayTags::Get().Stance_Crouching)
				{
					OwnerCharacter->UnCrouch();
				}
			}
			else if (MovementState == FALSGameplayTags::Get().MovementState_Ragdoll)
			{
				ReplicatedRagdollEnd();
			}
		}
	}
	else
	{
		OwnerCharacter->StopJumping();
	}
}

void UALSComponent::SprintAction_Implementation(bool bValue)
{
	if (bValue)
	{
		SetDesiredGait(FALSGameplayTags::Get().Gait_Sprinting);
	}
	else
	{
		SetDesiredGait(FALSGameplayTags::Get().Gait_Running);
	}
}

void UALSComponent::AimAction_Implementation(bool bValue)
{
	if (bValue)
	{
		// AimAction: Hold "AimAction" to enter the aiming mode, release to revert back the desired rotation mode.
		SetRotationMode(FALSGameplayTags::Get().RotationMode_Aiming);
	}
	else
	{
		if (ViewMode == FALSGameplayTags::Get().ViewMode_ThirdPerson)
		{
			SetRotationMode(DesiredRotationMode);
		}
		else if (ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson)
		{
			SetRotationMode(FALSGameplayTags::Get().RotationMode_LookingDirection);
		}
	}
}

void UALSComponent::CameraTapAction_Implementation()
{
	if (ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson)
	{
		// Don't swap shoulders on first person mode
		return;
	}

	// Switch shoulders
	SetRightShoulder(!bRightShoulder);
}

void UALSComponent::CameraHeldAction_Implementation()
{
	// Switch camera mode
	if (ViewMode == FALSGameplayTags::Get().ViewMode_FirstPerson)
	{
		SetViewMode(FALSGameplayTags::Get().ViewMode_ThirdPerson);
	}
	else if (ViewMode == FALSGameplayTags::Get().ViewMode_ThirdPerson)
	{
		SetViewMode(FALSGameplayTags::Get().ViewMode_FirstPerson);
	}
}

void UALSComponent::StanceAction_Implementation()
{
	// Stance Action: Press "Stance Action" to toggle Standing / Crouching, double tap to Roll.

	if (MovementAction != FALSGameplayTags::Get().MovementAction_None)
	{
		return;
	}

	UWorld* World = GetWorld();
	check(World);

	const float PrevStanceInputTime = LastStanceInputTime;
	LastStanceInputTime = World->GetTimeSeconds();

	if (LastStanceInputTime - PrevStanceInputTime <= RollDoubleTapTimeout)
	{
		// Roll
		Replicated_PlayMontage(GetRollAnimation(), 1.15f);

		if (Stance == FALSGameplayTags::Get().Stance_Standing)
		{
			SetDesiredStance(FALSGameplayTags::Get().Stance_Crouching);
		}
		else if (Stance == FALSGameplayTags::Get().Stance_Crouching)
		{
			SetDesiredStance(FALSGameplayTags::Get().Stance_Standing);
		}
		return;
	}

	if (MovementState == FALSGameplayTags::Get().MovementState_Grounded)
	{
		if (Stance == FALSGameplayTags::Get().Stance_Standing)
		{
			SetDesiredStance(FALSGameplayTags::Get().Stance_Crouching);
			OwnerCharacter->Crouch();
		}
		else if (Stance == FALSGameplayTags::Get().Stance_Crouching)
		{
			SetDesiredStance(FALSGameplayTags::Get().Stance_Standing);
			OwnerCharacter->UnCrouch();
		}
	}

	// Notice: MovementState == FALSGameplayTags::Get().MovementState_InAir case is removed
}

void UALSComponent::WalkAction_Implementation()
{
	if (DesiredGait == FALSGameplayTags::Get().Gait_Walking)
	{
		SetDesiredGait(FALSGameplayTags::Get().Gait_Running);
	}
	else if (DesiredGait == FALSGameplayTags::Get().Gait_Running)
	{
		SetDesiredGait(FALSGameplayTags::Get().Gait_Walking);
	}
}

void UALSComponent::RagdollAction_Implementation()
{
	// Ragdoll Action: Press "Ragdoll Action" to toggle the ragdoll state on or off.

	if (GetMovementState() == FALSGameplayTags::Get().MovementState_Ragdoll)
	{
		ReplicatedRagdollEnd();
	}
	else
	{
		ReplicatedRagdollStart();
	}
}

void UALSComponent::VelocityDirectionAction_Implementation()
{
	// Select Rotation Mode: Switch the desired (default) rotation mode to Velocity or Looking Direction.
	// This will be the mode the character reverts back to when un-aiming
	SetDesiredRotationMode(FALSGameplayTags::Get().RotationMode_VelocityDirection);
	SetRotationMode(FALSGameplayTags::Get().RotationMode_VelocityDirection);
}

void UALSComponent::LookingDirectionAction_Implementation()
{
	SetDesiredRotationMode(FALSGameplayTags::Get().RotationMode_LookingDirection);
	SetRotationMode(FALSGameplayTags::Get().RotationMode_LookingDirection);
}

void UALSComponent::ReplicatedRagdollStart()
{
	if (OwnerCharacter->HasAuthority())
	{
		Multicast_RagdollStart();
	}
	else
	{
		Server_RagdollStart();
	}
}

void UALSComponent::ReplicatedRagdollEnd()
{
	if (OwnerCharacter->HasAuthority())
	{
		Multicast_RagdollEnd(OwnerCharacter->GetActorLocation());
	}
	else
	{
		Server_RagdollEnd(OwnerCharacter->GetActorLocation());
	}
}

void UALSComponent::OnRep_RotationMode(FGameplayTag PrevRotMode)
{
	OnRotationModeChanged(PrevRotMode);
	OnRotationModeChangedEvent.Broadcast(PrevRotMode);
}

void UALSComponent::OnRep_ViewMode(FGameplayTag PrevViewMode)
{
	OnViewModeChanged(PrevViewMode);
	OnViewModeChangedEvent.Broadcast(PrevViewMode);
}

void UALSComponent::OnRep_OverlayState(FGameplayTag PrevOverlayState)
{
	OnOverlayStateChanged(PrevOverlayState);
	OnOverlayStateChangedEvent.Broadcast(PrevOverlayState);

}

void UALSComponent::OnRep_VisibleMesh(const USkeletalMesh* PreviousSkeletalMesh)
{
	OnVisibleMeshChanged(PreviousSkeletalMesh);
}
