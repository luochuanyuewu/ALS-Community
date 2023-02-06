// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ALSControlComponent.h"

#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "Character/ALSComponent.h"
#include "Character/ALSPlayerCameraManager.h"
#include "Components/ALSDebugComponent.h"
#include "GameFramework/Character.h"


// Sets default values for this component's properties
UALSControlComponent::UALSControlComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UALSControlComponent::OnRegister()
{
	Super::OnRegister();
	if (APlayerController* PC =  Cast<APlayerController>(GetOwner()))
	{
		OwnerController = PC;
		PC->OnPossessedPawnChanged.AddDynamic(this,&ThisClass::UALSControlComponent::OnPossessedPawnChanged);
	}
}

void UALSControlComponent::OnUnregister()
{
	if (APlayerController* PC =  Cast<APlayerController>(GetOwner()))
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this,&ThisClass::UALSControlComponent::OnPossessedPawnChanged);
	}
	Super::OnUnregister();
}



void UALSControlComponent::OnPossessedPawnChanged_Implementation(APawn* OldPawn, APawn* NewPawn)
{
	if (bAlreadySetup)
		return;
	PossessedCharacter = Cast<ACharacter>(NewPawn);
	if (!IsRunningDedicatedServer())
	{
		// Servers want to setup camera only in listen servers.
		SetupCamera();
	}

	SetupInputs();

	UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
	if (DebugComp)
	{
		DebugComp->OnPlayerControllerInitialized(Cast<APlayerController>(GetOwner()));
	}
	bAlreadySetup = true;
}

void UALSControlComponent::SetupInputComponent(UInputComponent* InputComponent)
{
	if (bAlreadyBoundInput)
		return;
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindActions(DefaultInputMappingContext);
		BindActions(DebugInputMappingContext);
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("ALS Community requires Enhanced Input System to be activated in project settings to function properly"));
	}

	bAlreadyBoundInput = true;
}

void UALSControlComponent::OnCheckInputComponent()
{
	if (OwnerController!= nullptr && OwnerController->InputComponent != nullptr)
	{
		if (bAlreadySetup)
		{
			Timer_CheckInputComponent.Invalidate();
			SetupInputComponent(OwnerController->InputComponent);
		}
	}
}


void UALSControlComponent::BindActions(UInputMappingContext* Context)
{
	if (Context && OwnerController)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Context->GetMappings();
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(OwnerController->InputComponent);
		if (EnhancedInputComponent)
		{
			// There may be more than one keymapping assigned to one action. So, first filter duplicate action entries to prevent multiple delegate bindings
			TSet<const UInputAction*> UniqueActions;
			for (const FEnhancedActionKeyMapping& Keymapping : Mappings)
			{
				UniqueActions.Add(Keymapping.Action);
			}
			for (const UInputAction* UniqueAction : UniqueActions)
			{
				EnhancedInputComponent->BindAction(UniqueAction, ETriggerEvent::Triggered, Cast<UObject>(this), UniqueAction->GetFName());
			}
		}
	}
}

void UALSControlComponent::BeginPlay()
{
	Super::BeginPlay();
	if (OwnerController)
	{
		if (OwnerController->GetPawn() != nullptr)
		{
			OnPossessedPawnChanged(nullptr,OwnerController->GetPawn());	
		}
	}
	if (!bAlreadyBoundInput)
	{
		GetWorld()->GetTimerManager().SetTimer(Timer_CheckInputComponent,FTimerDelegate::CreateUObject(this,&ThisClass::OnCheckInputComponent),0.2f,true);
	}
}

void UALSControlComponent::SetupInputs()
{
	if (PossessedCharacter && OwnerController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(OwnerController->GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bForceImmediately = 1;
			Subsystem->AddMappingContext(DefaultInputMappingContext, 1, Options);
			UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
			if (DebugComp)
			{
				// Do only if we have debug component
				Subsystem->AddMappingContext(DebugInputMappingContext, 0, Options);
			}
		}
	}
}

void UALSControlComponent::SetupCamera()
{
	// Call "OnPossess" in Player Camera Manager when possessing a pawn
	AALSPlayerCameraManager* CastedMgr = Cast<AALSPlayerCameraManager>(OwnerController->PlayerCameraManager);
	if (PossessedCharacter && CastedMgr)
	{
		CastedMgr->OnPossess(PossessedCharacter);
	}
}

void UALSControlComponent::ForwardMovementAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->ForwardMovementAction(Value.GetMagnitude());
	}
}

void UALSControlComponent::RightMovementAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->RightMovementAction(Value.GetMagnitude());
	}
}

void UALSControlComponent::CameraUpAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->CameraUpAction(Value.GetMagnitude());
	}
}

void UALSControlComponent::CameraRightAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->CameraRightAction(Value.GetMagnitude());
	}
}

void UALSControlComponent::JumpAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->JumpAction(Value.Get<bool>());
	}
}

void UALSControlComponent::SprintAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->SprintAction(Value.Get<bool>());
	}
}

void UALSControlComponent::AimAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->AimAction(Value.Get<bool>());
	}
}

void UALSControlComponent::CameraTapAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->CameraTapAction();
	}
}

void UALSControlComponent::CameraHeldAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->CameraHeldAction();
	}
}

void UALSControlComponent::StanceAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->StanceAction();
	}
}

void UALSControlComponent::WalkAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->WalkAction();
	}
}

void UALSControlComponent::RagdollAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->RagdollAction();
	}
}

void UALSControlComponent::VelocityDirectionAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->VelocityDirectionAction();
	}
}

void UALSControlComponent::LookingDirectionAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		if (UALSComponent* ALS = UALSComponent::FindALSComponent(PossessedCharacter))
			ALS->LookingDirectionAction();
	}
}

void UALSControlComponent::DebugToggleHudAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleHud();
		}
	}
}

void UALSControlComponent::DebugToggleDebugViewAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleDebugView();
		}
	}
}

void UALSControlComponent::DebugToggleTracesAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleTraces();
		}
	}
}

void UALSControlComponent::DebugToggleShapesAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleDebugShapes();
		}
	}
}

void UALSControlComponent::DebugToggleLayerColorsAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleLayerColors();
		}
	}
}

void UALSControlComponent::DebugToggleCharacterInfoAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleCharacterInfo();
		}
	}
}

void UALSControlComponent::DebugToggleSlomoAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleSlomo();
		}
	}
}

void UALSControlComponent::DebugFocusedCharacterCycleAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->FocusedDebugCharacterCycle(Value.GetMagnitude() > 0);
		}
	}
}

void UALSControlComponent::DebugToggleMeshAction(const FInputActionValue& Value)
{
	if (PossessedCharacter && Value.Get<bool>())
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->ToggleDebugMesh();
		}
	}
}

void UALSControlComponent::DebugOpenOverlayMenuAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->OpenOverlayMenu(Value.Get<bool>());
		}
	}
}

void UALSControlComponent::DebugOverlayMenuCycleAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		UALSDebugComponent* DebugComp = Cast<UALSDebugComponent>(PossessedCharacter->GetComponentByClass(UALSDebugComponent::StaticClass()));
		if (DebugComp)
		{
			DebugComp->OverlayMenuCycle(Value.GetMagnitude() > 0);
		}
	}
}
