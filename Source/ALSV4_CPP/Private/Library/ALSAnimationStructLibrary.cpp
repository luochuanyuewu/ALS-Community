// Fill out your copyright notice in the Description page of Project Settings.


#include "Library/ALSAnimationStructLibrary.h"

#include "ALSGameplayTags.h"


FALSAnimCharacterInformation::FALSAnimCharacterInformation()
{
	PrevMovementState = FALSGameplayTags::Get().MovementState_None;
	ViewMode = FALSGameplayTags::Get().ViewMode_ThirdPerson;
}
