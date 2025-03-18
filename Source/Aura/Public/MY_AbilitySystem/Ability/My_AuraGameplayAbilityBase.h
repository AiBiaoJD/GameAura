// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "My_AuraGameplayAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="My_Input")
	FGameplayTag StartUpInputTag;
};
