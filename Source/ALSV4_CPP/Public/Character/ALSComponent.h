// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Library/ALSCharacterStructLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "ALSComponent.generated.h"


class UALSCharacterMovementComponent;
// forward declarations
class UALSDebugComponent;
class UAnimMontage;
class UALSPlayerCameraBehaviorV2;
enum class EVisibilityBasedAnimTickOption : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALSJumpPressedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FALSJumpedSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSBoolStateSignature, bool, bState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FALSTagChangedSignature, FGameplayTag, Tag);

/*
 * Base advanced locomotion component.
 */
UCLASS(ClassGroup="ALS", BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent))
class ALSV4_CPP_API UALSComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UALSComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintPure, Category = "ALS")
	static UALSComponent* FindALSComponent(const AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ALS", meta=(DisplayName="Find ALSComponent", ExpandBoolAsExecs = "ReturnValue"))
	static bool K2_FindALSComponent(const AActor* Actor, UALSComponent*& Instance);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void BeginPlay() override;

	virtual void OnRegister() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


	// should called when character owner startCrouch. TODO ALS
	UFUNCTION(BlueprintCallable, Category = "ALS")
	virtual void OnOwnerCharacterStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	// should called when character owner endCrouch TODO ALS
	UFUNCTION(BlueprintCallable, Category = "ALS")
	virtual void OnOwnerCharacterEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

	// should called when character owner Jumped TODO ALS
	UFUNCTION(BlueprintCallable, Category = "ALS")
	virtual void OnOwnerCharacterJumped();

	// should called when character owner Landed TODO ALS
	UFUNCTION(BlueprintCallable, Category = "ALS")
	virtual void OnOwnerCharacterLanded(const FHitResult& Hit);


	// virtual void PostInitializeComponents() override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	ACharacter* GetCharacterOwner() const { return OwnerCharacter; }

	/** Ragdoll System */

	/** Implement on BP to get required get up animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Ragdoll System")
	UAnimMontage* GetGetUpAnimation(bool bRagdollFaceUpState);


	virtual FName GetRagdollBoneName() const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "ALS|Ragdoll System")
	virtual void RagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Unreliable, Category = "ALS|Ragdoll System")
	void Server_SetMeshLocationDuringRagdoll(FVector MeshLocation);

	/** Character States */

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetMovementState(UPARAM(meta=(Categories="ALS.MovementState")) FGameplayTag NewState, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetPrevMovementState() const { return PrevMovementState; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetMovementAction(UPARAM(meta=(Categories="ALS.MovementAction")) FGameplayTag NewAction, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetMovementAction() const { return MovementAction; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetStance(UPARAM(meta=(Categories="ALS.Stance")) FGameplayTag NewStance, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetStance() const { return Stance; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetOverlayOverrideState(int32 NewState);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	int32 GetOverlayOverrideState() const { return OverlayOverrideState; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetGait(UPARAM(meta=(Categories="ALS.Gait")) FGameplayTag NewGait, bool bForce = false);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetGait() const { return Gait; }

	UFUNCTION(BlueprintGetter, Category = "ALS|CharacterStates")
	FGameplayTag GetDesiredGait() const { return DesiredGait; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetRotationMode(UPARAM(meta=(Categories="ALS.RotationMode")) FGameplayTag NewRotationMode, bool bForce = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetRotationMode(UPARAM(meta=(Categories="ALS.RotationMode")) FGameplayTag NewRotationMode, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetRotationMode() const { return RotationMode; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetViewMode(UPARAM(meta=(Categories="ALS.ViewMode")) FGameplayTag NewViewMode, bool bForce = false);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetViewMode(UPARAM(meta=(Categories="ALS.ViewMode")) FGameplayTag NewViewMode, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetViewMode() const { return ViewMode; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetOverlayState(UPARAM(meta=(Categories="ALS.OverlayState")) FGameplayTag NewState, bool bForce = false);

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetGroundedEntryState(UPARAM(meta=(Categories="ALS.GroundedEntryState")) FGameplayTag NewState);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetOverlayState(UPARAM(meta=(Categories="ALS.OverlayState")) FGameplayTag NewState, bool bForce);

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetOverlayState() const { return OverlayState; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Character States")
	FGameplayTag GetGroundedEntryState() const { return GroundedEntryState; }

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnMovementStateChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnMovementActionChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnStanceChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnRotationModeChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnGaitChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnViewModeChangedEvent;

	UPROPERTY(BlueprintAssignable)
	FALSTagChangedSignature OnOverlayStateChangedEvent;

	// UPROPERTY(BlueprintAssignable)
	// FALSTagChangedSignature OnVisibleMeshEvent;

	UPROPERTY(BlueprintAssignable)
	FALSBoolStateSignature OnShoulderChangedEvent;


	/** Landed, Jumped, Rolling, Mantling and Ragdoll*/
	/** On Landed*/
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	virtual void EventOnLanded();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_OnLanded();

	/** On Jumped*/
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void EventOnJumped();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_OnJumped();

	/** Rolling Montage Play Replication*/
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_PlayMontage(UAnimMontage* Montage, float PlayRate);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_PlayMontage(UAnimMontage* Montage, float PlayRate);

	/** Ragdolling*/
	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void ReplicatedRagdollStart();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_RagdollStart();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_RagdollStart();

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void ReplicatedRagdollEnd();

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_RagdollEnd(FVector CharacterLocation);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "ALS|Character States")
	void Multicast_RagdollEnd(FVector CharacterLocation);

	/** Input */

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FALSJumpPressedSignature JumpPressedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FALSJumpedSignature OnJumpedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ALS|Input")
	FALSBoolStateSignature RagdollStateChangedDelegate;

	UFUNCTION(BlueprintGetter, Category = "ALS|Input")
	FGameplayTag GetDesiredStance() const { return DesiredStance; }

	UFUNCTION(BlueprintSetter, Category = "ALS|Input")
	void SetDesiredStance(UPARAM(meta=(Categories="ALS.Stance")) FGameplayTag NewStance);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Input")
	void Server_SetDesiredStance(UPARAM(meta=(Categories="ALS.Stance")) FGameplayTag NewStance);

	UFUNCTION(BlueprintCallable, Category = "ALS|Character States")
	void SetDesiredGait(UPARAM(meta=(Categories="ALS.Gait")) FGameplayTag NewGait);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetDesiredGait(UPARAM(meta=(Categories="ALS.Gait")) FGameplayTag NewGait);

	UFUNCTION(BlueprintGetter, Category = "ALS|Input")
	FGameplayTag GetDesiredRotationMode() const { return DesiredRotationMode; }

	UFUNCTION(BlueprintSetter, Category = "ALS|Input")
	void SetDesiredRotationMode(UPARAM(meta=(Categories="ALS.RotationMode")) FGameplayTag NewRotMode);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Character States")
	void Server_SetDesiredRotationMode(UPARAM(meta=(Categories="ALS.RotationMode")) FGameplayTag NewRotMode);

	/** Rotation System */

	UFUNCTION(BlueprintCallable, Category = "ALS|Rotation System")
	void SetActorLocationAndTargetRotation(FVector NewLocation, FRotator NewRotation);

	/** Movement System */

	UFUNCTION(BlueprintGetter, Category = "ALS|Movement System")
	bool HasMovementInput() const { return bHasMovementInput; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	virtual FALSMovementSettings GetTargetMovementSettings() const;

	/**
	 *根据旋转模式和态势等数据、对DesiredGait作修正。不出意外返回的都是DesiredGait
	 */
	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	FGameplayTag GetAllowedGait() const;

	//根据移速拿到实际的步态
	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	FGameplayTag GetActualGait(UPARAM(meta=(Categories="ALS.Gait")) FGameplayTag AllowedGait) const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Movement System")
	virtual bool CanSprint() const;

	/** BP implementable function that called when Breakfall starts */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Movement System")
	void OnBreakfall();
	virtual void OnBreakfall_Implementation();

	/** BP implementable function that called when A Montage starts, e.g. during rolling */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Movement System")
	void Replicated_PlayMontage(UAnimMontage* Montage, float PlayRate);
	virtual void Replicated_PlayMontage_Implementation(UAnimMontage* Montage, float PlayRate);

	/** Implement on BP to get required roll animation according to character's state */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "ALS|Movement System")
	UAnimMontage* GetRollAnimation();

	/** Utility */

	UFUNCTION(BlueprintCallable, Category = "ALS|Utility")
	float GetAnimCurveValue(FName CurveName) const;

	UFUNCTION(BlueprintCallable, Category = "ALS|Utility")
	void SetVisibleMesh(USkeletalMesh* NewSkeletalMesh);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "ALS|Utility")
	void Server_SetVisibleMesh(USkeletalMesh* NewSkeletalMesh);

	/** Camera System */

	UFUNCTION(BlueprintGetter, Category = "ALS|Camera System")
	bool IsRightShoulder() const { return bRightShoulder; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void SetRightShoulder(bool bNewRightShoulder);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual ECollisionChannel GetThirdPersonTraceParams(FVector& TraceOrigin, float& TraceRadius);

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FTransform GetThirdPersonPivotTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	virtual FVector GetFirstPersonCameraTarget();

	UFUNCTION(BlueprintCallable, Category = "ALS|Camera System")
	void GetCameraParameters(float& TPFOVOut, float& FPFOVOut, bool& bRightShoulderOut) const;

	/** Essential Information Getters/Setters */

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	FVector GetAcceleration() const { return Acceleration; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	bool IsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable, Category = "ALS|Essential Information")
	FVector GetMovementInput() const;

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	FRotator GetLastVelocityRotation() const { return LastVelocityRotation; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	FRotator GetLastMovementInputRotation() const { return LastMovementInputRotation; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetMovementInputAmount() const { return MovementInputAmount; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	bool GetIsMoving() const { return bIsMoving; }

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ALS|Essential Information")
	FRotator GetAimingRotation() const;
	FRotator GetAimingRotation_Implementation() const { return AimingRotation; }

	UFUNCTION(BlueprintGetter, Category = "ALS|Essential Information")
	float GetAimYawRate() const { return AimYawRate; }

	/** Input */

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void ForwardMovementAction(float Value);
	virtual void ForwardMovementAction_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void RightMovementAction(float Value);
	virtual void RightMovementAction_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraUpAction(float Value);
	virtual void CameraUpAction_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraRightAction(float Value);
	virtual void CameraRightAction_Implementation(float Value);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void JumpAction(bool bValue);
	virtual void JumpAction_Implementation(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void SprintAction(bool bValue);
	virtual void SprintAction_Implementation(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void AimAction(bool bValue);
	virtual void AimAction_Implementation(bool bValue);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraTapAction();
	virtual void CameraTapAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void CameraHeldAction();
	virtual void CameraHeldAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void StanceAction();
	virtual void StanceAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void WalkAction();
	virtual void WalkAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void RagdollAction();
	virtual void RagdollAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void VelocityDirectionAction();
	virtual void VelocityDirectionAction_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ALS|Input")
	void LookingDirectionAction();
	virtual void LookingDirectionAction_Implementation();

	/** Replicated Essential Information*/

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float EasedMaxAcceleration = 0.0f;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FVector ReplicatedCurrentAcceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Essential Information")
	FRotator ReplicatedControlRotation = FRotator::ZeroRotator;


	virtual void SetMovementModel(const FDataTableRowHandle& InModel);

protected:
	/** Ragdoll System */

	void RagdollUpdate(float DeltaTime);

	void SetActorLocationDuringRagdoll(float DeltaTime);

	/** State Changes */
	UFUNCTION()
	virtual void OnMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0);

	virtual void OnMovementStateChanged(FGameplayTag PreviousState);

	virtual void OnMovementActionChanged(FGameplayTag PreviousAction);

	virtual void OnStanceChanged(FGameplayTag PreviousStance);

	virtual void OnRotationModeChanged(FGameplayTag PreviousRotationMode);

	virtual void OnGaitChanged(FGameplayTag PreviousGait);

	virtual void OnViewModeChanged(FGameplayTag PreviousViewMode);

	virtual void OnOverlayStateChanged(FGameplayTag PreviousState);

	virtual void OnVisibleMeshChanged(const USkeletalMesh* PreviousSkeletalMesh);

	void OnLandFrictionReset();

	virtual void SetEssentialValues(float DeltaTime);

	void UpdateCharacterMovement();

	virtual void UpdateGroundedRotation(float DeltaTime);

	virtual void UpdateInAirRotation(float DeltaTime);

	/** Utils */

	void SmoothCharacterRotation(FRotator Target, float TargetInterpSpeed, float ActorInterpSpeed, float DeltaTime);

	virtual float CalculateGroundedRotationRate() const;

	void LimitRotation(float AimYawMin, float AimYawMax, float InterpSpeed, float DeltaTime);

	void ForceUpdateCharacterState();

	/** Replication */
	UFUNCTION(Category = "ALS|Replication")
	void OnRep_RotationMode(FGameplayTag PrevRotMode);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_ViewMode(FGameplayTag PrevViewMode);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_OverlayState(FGameplayTag PrevOverlayState);

	UFUNCTION(Category = "ALS|Replication")
	void OnRep_VisibleMesh(const USkeletalMesh* PreviousSkeletalMesh);

protected:
	/* Custom movement component*/
	UPROPERTY()
	TObjectPtr<UALSCharacterMovementComponent> MyCharacterMovementComponent;

	/** Input */

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input", meta=(Categories="ALS.RotationMode"))
	FGameplayTag DesiredRotationMode;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "ALS|Input", meta=(Categories="ALS.Gait"))
	FGameplayTag DesiredGait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "ALS|Input", meta=(Categories="ALS.Stance"))
	FGameplayTag DesiredStance;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookUpDownRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float LookLeftRightRate = 1.25f;

	UPROPERTY(EditDefaultsOnly, Category = "ALS|Input", BlueprintReadOnly)
	float RollDoubleTapTimeout = 0.3f;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bBreakFall = false;

	UPROPERTY(Category = "ALS|Input", BlueprintReadOnly)
	bool bSprintHeld = false;

	/** Camera System */

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float ThirdPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	float FirstPersonFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Camera System")
	bool bRightShoulder = false;

	/** Movement System */

	/** 勾选后直接通过MovementData配置移动,否则通过表. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ALS|Movement System")
	bool bCustomMovementData = true;

	/** 从UEMovementStateSettings表里选择一种运动配置 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|Movement System")
	FDataTableRowHandle MovementModel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Movement System", meta=(EditCondition="bCustomMovementData", EditConditionHides))
	FALSMovementStateSettings MovementData;

	/** Essential Information */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FVector Acceleration = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	bool bHasMovementInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastVelocityRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	FRotator LastMovementInputRotation;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float Speed = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float MovementInputAmount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Essential Information")
	float AimYawRate = 0.0f;


	/** Replicated Skeletal Mesh Information*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|Skeletal Mesh", ReplicatedUsing = OnRep_VisibleMesh)
	TObjectPtr<USkeletalMesh> VisibleMesh = nullptr;

	/** State Values */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_OverlayState, meta=(Categories="ALS.OverlayState"))
	FGameplayTag OverlayState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	FGameplayTag GroundedEntryState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	FGameplayTag MovementState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	FGameplayTag PrevMovementState;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	FGameplayTag MovementAction;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_RotationMode)
	FGameplayTag RotationMode;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	FGameplayTag Gait;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values", meta=(Categories="ALS.Stance"))
	FGameplayTag Stance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ALS|State Values", ReplicatedUsing = OnRep_ViewMode, meta=(Categories="ALS.ViewMode"))
	FGameplayTag ViewMode;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|State Values")
	int32 OverlayOverrideState = 0;

	/** Rotation System */

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	FRotator InAirRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Rotation System")
	float YawOffset = 0.0f;

	/** Breakfall System */

	/** If player hits to the ground with a specified amount of velocity, switch to breakfall state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System")
	bool bBreakfallOnLand = true;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to breakfall state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Breakfall System", meta = (EditCondition ="bBreakfallOnLand"))
	float BreakfallOnLandVelocity = 700.0f;

	/** Ragdoll System */

	/** If the skeleton uses a reversed pelvis bone, flip the calculation operator */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bReversedPelvis = false;

	/** If player hits to the ground with a specified amount of velocity, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnLand = false;

	/** If player hits to the ground with an amount of velocity greater than specified value, switch to ragdoll state */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ALS|Ragdoll System", meta = (EditCondition ="bRagdollOnLand"))
	float RagdollOnLandVelocity = 1000.0f;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollOnGround = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	bool bRagdollFaceUp = false;

	UPROPERTY(BlueprintReadOnly, Category = "ALS|Ragdoll System")
	FVector LastRagdollVelocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "ALS|Ragdoll System")
	FVector TargetRagdollLocation = FVector::ZeroVector;

	/* Server ragdoll pull force storage*/
	float ServerRagdollPull = 0.0f;

	/* Dedicated server mesh default visibility based anim tick option*/
	EVisibilityBasedAnimTickOption DefVisBasedTickOp;
	ECollisionChannel PrevCollisionObjectType;

	bool bPreRagdollURO = false;

	/** Cached Variables */

	FVector PreviousVelocity = FVector::ZeroVector;

	float PreviousAimYaw = 0.0f;

	/** Last time the 'first' crouch/roll button is pressed */
	float LastStanceInputTime = 0.0f;

	/* Timer to manage reset of braking friction factor after on landed event */
	FTimerHandle OnLandedFrictionResetTimer;

	/* Smooth out aiming by interping control rotation*/
	FRotator AimingRotation = FRotator::ZeroRotator;

	/** We won't use curve based movement and a few other features on networked games */
	bool bEnableNetworkOptimizations = false;

private:
	UPROPERTY()
	TObjectPtr<UALSDebugComponent> ALSDebugComponent = nullptr;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter = nullptr;
};
