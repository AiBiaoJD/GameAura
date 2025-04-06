// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "MyAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMyAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
