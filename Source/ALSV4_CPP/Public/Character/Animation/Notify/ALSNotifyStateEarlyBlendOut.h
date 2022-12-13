// Copyright:       Copyright (C) 2022 Doğa Can Yanıkoğlu
// Source Code:     https://github.com/dyanikoglu/ALS-Community


#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ALSNotifyStateEarlyBlendOut.generated.h"

/**
 * Character early blend out anim state
 */
UCLASS()
class ALSV4_CPP_API UALSNotifyStateEarlyBlendOut : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
	                        const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

public:

	UALSNotifyStateEarlyBlendOut();

	// virtual void PostInitProperties() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	TObjectPtr<UAnimMontage> ThisMontage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	float BlendOutTime = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bCheckMovementState = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, meta=(Categories="ALS.MovementState"))
	FGameplayTag MovementState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bCheckStance = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify, meta=(Categories="ALS.Stance"))
	FGameplayTag Stance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimNotify)
	bool bCheckMovementInput = false;
};
