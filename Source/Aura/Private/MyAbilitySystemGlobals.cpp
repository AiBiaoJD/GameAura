// Copyright ABiao


#include "MyAbilitySystemGlobals.h"
#include "My_AuraAbilityTypes.h"

FGameplayEffectContext* UMyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FMY_AuraGamePlayEffectContext();
}
