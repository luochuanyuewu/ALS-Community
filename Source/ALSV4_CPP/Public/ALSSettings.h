// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ALSSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DisplayName="Advanced Locomotion Settings")
class ALSV4_CPP_API UALSSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable,BlueprintPure, Category = ALS, meta=(DisplayName="Advanced Locomotion Settings"))
	static const UALSSettings* Get();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_FP_Camera = TEXT("FP_Camera");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Pelvis = TEXT("Pelvis");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_RagdollPose = TEXT("RagdollPose");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_RotationAmount = TEXT("RotationAmount");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_YawOffset = TEXT("YawOffset");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_root = TEXT("root");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_spine_03= TEXT("spine_03");


	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_BasePose_CLF = TEXT("BasePose_CLF");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_BasePose_N = TEXT("BasePose_N");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Enable_FootIK_R = TEXT("Enable_FootIK_R");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Enable_FootIK_L = TEXT("Enable_FootIK_L");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Enable_HandIK_L = TEXT("Enable_HandIK_L");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Enable_HandIK_R = TEXT("Enable_HandIK_R");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Enable_Transition = TEXT("Enable_Transition");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_FootLock_L = TEXT("FootLock_L");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_FootLock_R = TEXT("FootLock_R");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Grounded___Slot = TEXT("Grounded Slot");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_L = TEXT("Layering_Arm_L");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_L_Add = TEXT("Layering_Arm_L_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_L_LS = TEXT("Layering_Arm_L_LS");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_R = TEXT("Layering_Arm_R");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_R_Add = TEXT("Layering_Arm_R_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Arm_R_LS = TEXT("Layering_Arm_R_LS");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Hand_L = TEXT("Layering_Hand_L");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Hand_R = TEXT("Layering_Hand_R");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Head = TEXT("Layering_Head");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Head_Add = TEXT("Layering_Head_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Legs = TEXT("Layering_Legs");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Legs_Add = TEXT("Layering_Legs_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Pelvis = TEXT("Layering_Pelvis");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Pelvis_Add = TEXT("Layering_Pelvis_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Spine = TEXT("Layering_Spine");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Layering_Spine_Add = TEXT("Layering_Spine_Add");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Mask_AimOffset = TEXT("Mask_AimOffset");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_Mask_LandPrediction = TEXT("Mask_LandPrediction");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME__ALSCharacterAnimInstance__RotationAmount = TEXT("RotationAmount");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_VB___foot_target_l = TEXT("VB foot_target_l");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_VB___foot_target_r = TEXT("VB foot_target_r");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME_W_Gait = TEXT("W_Gait");
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="ALS")
	FName NAME__ALSCharacterAnimInstance__root = TEXT("root");
	

};
