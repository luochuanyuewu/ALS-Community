// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community

#pragma once

#include "CoreMinimal.h"
#include "ALSCharacterEnumLibrary.h"

#include "ALSStructEnumLibrary.generated.h"


USTRUCT(BlueprintType)
struct FALSMovementState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementState State = EALSMovementState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Grounded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool InAir_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Mantling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Ragdoll_ = false;

	//自定义开始

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Movement System")
	bool Riding_ = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Movement System")
	bool Climbing_ = false;

public:
	FALSMovementState()
	{
	}

	FALSMovementState(const EALSMovementState InitialState) { *this = InitialState; }

	const bool& None() const { return None_; }
	const bool& Grounded() const { return Grounded_; }
	const bool& InAir() const { return InAir_; }
	const bool& Mantling() const { return Mantling_; }
	const bool& Ragdoll() const { return Ragdoll_; }
	//自定义开始
	const bool& Riding() const { return Riding_; }
	const bool& Climbing() const { return Climbing_; }



	operator EALSMovementState() const { return State; }

	void operator=(const EALSMovementState NewState)
	{
		State = NewState;
		None_ = State == EALSMovementState::None;
		Grounded_ = State == EALSMovementState::Grounded;
		InAir_ = State == EALSMovementState::InAir;
		Mantling_ = State == EALSMovementState::Mantling;
		Ragdoll_ = State == EALSMovementState::Ragdoll;
		//自定义开始
		Riding_ = State == EALSMovementState::Riding;
		Climbing_ = State == EALSMovementState::Climbing;

	}
};

USTRUCT(BlueprintType)
struct FALSStance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	EALSStance Stance = EALSStance::Standing;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Standing_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Crouching_ = false;

public:
	FALSStance()
	{
	}

	FALSStance(const EALSStance InitialStance) { *this = InitialStance; }

	const bool& Standing() const { return Standing_; }
	const bool& Crouching() const { return Crouching_; }

	operator EALSStance() const { return Stance; }

	void operator=(const EALSStance NewStance)
	{
		Stance = NewStance;
		Standing_ = Stance == EALSStance::Standing;
		Crouching_ = Stance == EALSStance::Crouching;
	}
};

USTRUCT(BlueprintType)
struct FALSRotationMode
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	EALSRotationMode RotationMode = EALSRotationMode::VelocityDirection;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool VelocityDirection_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool LookingDirection_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Rotation System")
	bool Aiming_ = false;

public:
	FALSRotationMode()
	{
	}

	FALSRotationMode(const EALSRotationMode InitialRotationMode) { *this = InitialRotationMode; }

	const bool& VelocityDirection() const { return VelocityDirection_; }
	const bool& LookingDirection() const { return LookingDirection_; }
	const bool& Aiming() const { return Aiming_; }

	operator EALSRotationMode() const { return RotationMode; }

	void operator=(const EALSRotationMode NewRotationMode)
	{
		RotationMode = NewRotationMode;
		VelocityDirection_ = RotationMode == EALSRotationMode::VelocityDirection;
		LookingDirection_ = RotationMode == EALSRotationMode::LookingDirection;
		Aiming_ = RotationMode == EALSRotationMode::Aiming;
	}
};

USTRUCT(BlueprintType)
struct FALSMovementDirection
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementDirection MovementDirection = EALSMovementDirection::Forward;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Forward_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Right_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Left_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Backward_ = false;

public:
	FALSMovementDirection()
	{
	}

	FALSMovementDirection(const EALSMovementDirection InitialMovementDirection)
	{
		*this = InitialMovementDirection;
	}

	const bool& Forward() const { return Forward_; }
	const bool& Right() const { return Right_; }
	const bool& Left() const { return Left_; }
	const bool& Backward() const { return Backward_; }

	operator EALSMovementDirection() const { return MovementDirection; }

	void operator=(const EALSMovementDirection NewMovementDirection)
	{
		MovementDirection = NewMovementDirection;
		Forward_ = MovementDirection == EALSMovementDirection::Forward;
		Right_ = MovementDirection == EALSMovementDirection::Right;
		Left_ = MovementDirection == EALSMovementDirection::Left;
		Backward_ = MovementDirection == EALSMovementDirection::Backward;
	}
};

USTRUCT(BlueprintType)
struct FALSMovementAction
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSMovementAction Action = EALSMovementAction::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool LowMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool HighMantle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Rolling_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool GettingUp_ = false;

public:
	FALSMovementAction()
	{
	}

	FALSMovementAction(const EALSMovementAction InitialAction) { *this = InitialAction; }

	const bool& None() const { return None_; }
	const bool& LowMantle() const { return LowMantle_; }
	const bool& HighMantle() const { return HighMantle_; }
	const bool& Rolling() const { return Rolling_; }
	const bool& GettingUp() const { return GettingUp_; }

	operator EALSMovementAction() const { return Action; }

	void operator=(const EALSMovementAction NewAction)
	{
		Action = NewAction;
		None_ = Action == EALSMovementAction::None;
		LowMantle_ = Action == EALSMovementAction::LowMantle;
		HighMantle_ = Action == EALSMovementAction::HighMantle;
		Rolling_ = Action == EALSMovementAction::Rolling;
		GettingUp_ = Action == EALSMovementAction::GettingUp;
	}
};

USTRUCT(BlueprintType)
struct FALSGait
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	EALSGait Gait = EALSGait::Walking;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Walking_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Running_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Movement System")
	bool Sprinting_ = false;

public:
	FALSGait()
	{
	}

	FALSGait(const EALSGait InitialGait) { *this = InitialGait; }

	const bool& Walking() const { return Walking_; }
	const bool& Running() const { return Running_; }
	const bool& Sprinting() const { return Sprinting_; }

	operator EALSGait() const { return Gait; }

	void operator=(const EALSGait NewGait)
	{
		Gait = NewGait;
		Walking_ = Gait == EALSGait::Walking;
		Running_ = Gait == EALSGait::Running;
		Sprinting_ = Gait == EALSGait::Sprinting;
	}
};

USTRUCT(BlueprintType)
struct FALSOverlayState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	EALSOverlayState State = EALSOverlayState::Default;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Default_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Masculine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Feminine_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Injured_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool HandsTied_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Rifle_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool PistolOneHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool PistolTwoHanded_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Bow_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Torch_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Binoculars_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Box_ = false;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Character States")
	bool Barrel_ = false;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Normal_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Normal_Flager_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Normal_Spear_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_EmptyHand_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Bow_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_CrossBow_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Spear_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Spear_Shield_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Short_S_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Short_D_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_Short_Shield_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool Fight_LongAxe_D_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Normal_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Fight_Bow_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Normal_Spear_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Fight_Short_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Fight_LongAxe_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Fight_Spear_ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category="Character States")
	bool H_Fight_CrossBow_ = false;

public:
	FALSOverlayState()
	{
	}

	FALSOverlayState(const EALSOverlayState InitialState) { *this = InitialState; }

	const bool& Default() const { return Default_; }
	const bool& Masculine() const { return Masculine_; }
	const bool& Feminine() const { return Feminine_; }
	const bool& Injured() const { return Injured_; }
	const bool& HandsTied() const { return HandsTied_; }
	const bool& Rifle() const { return Rifle_; }
	const bool& PistolOneHanded() const { return PistolOneHanded_; }
	const bool& PistolTwoHanded() const { return PistolTwoHanded_; }
	const bool& Bow() const { return Bow_; }
	const bool& Torch() const { return Torch_; }
	const bool& Binoculars() const { return Binoculars_; }
	const bool& Box() const { return Box_; }
	const bool& Barrel() const { return Barrel_; }

	//自己项目的
	const bool& Normal() const { return Normal_; }
	const bool& Normal_Flager() const { return Normal_Flager_; }
	const bool& Normal_Spear() const { return Normal_Spear_; }
	const bool& Fight_EmptyHand() const { return Fight_EmptyHand_; }
	const bool& Fight_Bow() const { return Fight_Bow_; }
	const bool& Fight_CrossBow() const { return Fight_CrossBow_; }
	const bool& Fight_Spear() const { return Fight_Spear_; }
	const bool& Fight_Spear_Shield() const { return Fight_Spear_Shield_; }
	const bool& Fight_Short_S() const { return Fight_Short_S_; }
	const bool& Fight_Short_D() const { return Fight_Short_D_; }
	const bool& Fight_Short_Shield() const { return Fight_Short_Shield_; }
	const bool& Fight_LongAxe_D() const { return Fight_LongAxe_D_; }
	const bool& H_Normal() const { return H_Normal_; }
	const bool& H_Normal_Spear() const { return H_Normal_Spear_; }
	const bool& H_Fight_Bow() const { return H_Fight_Bow_; }
	const bool& H_Fight_Short() const { return H_Fight_Short_; }
	const bool& H_Fight_LongAxe() const { return H_Fight_LongAxe_; }
	const bool& H_Fight_Spear() const { return H_Fight_Spear_; }
	const bool& H_Fight_CrossBow() const { return H_Fight_CrossBow_; }

	operator EALSOverlayState() const { return State; }

	void operator=(const EALSOverlayState NewAction)
	{
		State = NewAction;
		Default_ = State == EALSOverlayState::Default;
		Masculine_ = State == EALSOverlayState::Masculine;
		Feminine_ = State == EALSOverlayState::Feminine;
		Injured_ = State == EALSOverlayState::Injured;
		HandsTied_ = State == EALSOverlayState::HandsTied;
		Rifle_ = State == EALSOverlayState::Rifle;
		PistolOneHanded_ = State == EALSOverlayState::PistolOneHanded;
		PistolTwoHanded_ = State == EALSOverlayState::PistolTwoHanded;
		Bow_ = State == EALSOverlayState::Bow;
		Torch_ = State == EALSOverlayState::Torch;
		Binoculars_ = State == EALSOverlayState::Binoculars;
		Box_ = State == EALSOverlayState::Box;
		Barrel_ = State == EALSOverlayState::Barrel;

		//自己项目
		Barrel_ = State == EALSOverlayState::Barrel;
		Normal_ = State == EALSOverlayState::Normal;
		Normal_Flager_ = State == EALSOverlayState::Normal_Flager;
		Normal_Spear_ = State == EALSOverlayState::Normal_Spear;
		Fight_EmptyHand_ = State == EALSOverlayState::Fight_EmptyHand;
		Fight_Bow_ = State == EALSOverlayState::Fight_Bow;
		Fight_CrossBow_ = State == EALSOverlayState::Fight_CrossBow;
		Fight_Spear_ = State == EALSOverlayState::Fight_Spear;
		Fight_Spear_Shield_ = State == EALSOverlayState::Fight_Spear_Shield;
		Fight_Short_S_ = State == EALSOverlayState::Fight_Short_S;
		Fight_Short_D_ = State == EALSOverlayState::Fight_Short_D;
		Fight_Short_Shield_ = State == EALSOverlayState::Fight_Short_Shield;
		Fight_LongAxe_D_ = State == EALSOverlayState::Fight_LongAxe_D;
		H_Normal_ = State == EALSOverlayState::H_Normal;
		H_Normal_Spear_ = State == EALSOverlayState::H_Normal_Spear;
		H_Fight_Bow_ = State == EALSOverlayState::H_Fight_Bow;
		H_Fight_Short_ = State == EALSOverlayState::H_Fight_Short;
		H_Fight_LongAxe_ = State == EALSOverlayState::H_Fight_LongAxe;
		H_Fight_Spear_ = State == EALSOverlayState::H_Fight_Spear;
		H_Fight_CrossBow_ = State == EALSOverlayState::H_Fight_CrossBow;
	}
};

USTRUCT(BlueprintType)
struct FALSGroundedEntryState
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	EALSGroundedEntryState State = EALSGroundedEntryState::None;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	bool None_ = true;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "ALS|Breakfall System")
	bool Roll_ = false;

public:
	FALSGroundedEntryState()
	{
	}

	FALSGroundedEntryState(const EALSGroundedEntryState InitialState) { *this = InitialState; }

	const bool& None() const { return None_; }
	const bool& Roll() const { return Roll_; }

	operator EALSGroundedEntryState() const { return State; }

	void operator=(const EALSGroundedEntryState NewState)
	{
		State = NewState;
		None_ = State == EALSGroundedEntryState::None;
		Roll_ = State == EALSGroundedEntryState::Roll;
	}
};
