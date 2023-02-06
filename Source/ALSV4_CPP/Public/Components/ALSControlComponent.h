// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ALSControlComponent.generated.h"


UCLASS(ClassGroup=(ALS), meta=(BlueprintSpawnableComponent))
class ALSV4_CPP_API UALSControlComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UALSControlComponent();

	virtual void BindActions(class UInputMappingContext* Context);

	virtual void BeginPlay() override;

protected:
	// Called when the game starts
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPossessedPawnChanged( APawn* OldPawn, APawn* NewPawn);
	virtual void OnPossessedPawnChanged_Implementation( APawn* OldPawn, APawn* NewPawn);

	UFUNCTION()
	virtual void SetupInputComponent(UInputComponent* InputComponent);

	UFUNCTION()
	void OnCheckInputComponent();
	
protected:
	void SetupInputs();

	void SetupCamera();

	UFUNCTION()
	void ForwardMovementAction(const FInputActionValue& Value);

	UFUNCTION()
	void RightMovementAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraUpAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraRightAction(const FInputActionValue& Value);

	UFUNCTION()
	void JumpAction(const FInputActionValue& Value);

	UFUNCTION()
	void SprintAction(const FInputActionValue& Value);

	UFUNCTION()
	void AimAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraTapAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraHeldAction(const FInputActionValue& Value);

	UFUNCTION()
	void StanceAction(const FInputActionValue& Value);

	UFUNCTION()
	void WalkAction(const FInputActionValue& Value);

	UFUNCTION()
	void RagdollAction(const FInputActionValue& Value);

	UFUNCTION()
	void VelocityDirectionAction(const FInputActionValue& Value);

	UFUNCTION()
	void LookingDirectionAction(const FInputActionValue& Value);

	// Debug actions
	UFUNCTION()
	void DebugToggleHudAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleDebugViewAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleTracesAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleShapesAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleLayerColorsAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleCharacterInfoAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleSlomoAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugFocusedCharacterCycleAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugToggleMeshAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugOpenOverlayMenuAction(const FInputActionValue& Value);

	UFUNCTION()
	void DebugOverlayMenuCycleAction(const FInputActionValue& Value);

public:

	UPROPERTY()
	TObjectPtr<APlayerController> OwnerController = nullptr;
	
	/** Main character reference */
	UPROPERTY(BlueprintReadOnly, Category = "ALS")
	TObjectPtr<ACharacter> PossessedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Input")
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ALS|Input")
	TObjectPtr<UInputMappingContext> DebugInputMappingContext = nullptr;


protected:
	bool bAlreadySetup = false;
	bool bAlreadyBoundInput = false;

	FTimerHandle Timer_CheckInputComponent;

};


