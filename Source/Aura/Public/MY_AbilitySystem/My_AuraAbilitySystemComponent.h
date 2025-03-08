// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "My_AuraAbilitySystemComponent.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAbilitySystemComponent : public UAuraAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void AbilityActorInfoSet();

protected:

	//OnGameplayEffectAppliedDelegateToSelf 委托的回调函数,需要进行绑定
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
