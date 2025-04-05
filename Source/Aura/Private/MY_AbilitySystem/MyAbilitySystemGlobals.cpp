// Copyright ABiao


#include "MY_AbilitySystem/MyAbilitySystemGlobals.h"

#include "My_AuraAbilityTypes.h"

FGameplayEffectContext* UMyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMY_AuraGamePlayEffectContext();
}
