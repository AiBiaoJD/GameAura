// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "My_AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_EffectAssetTags, const FGameplayTagContainer& /*AssetTags*/)
/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAbilitySystemComponent : public UAuraAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UMy_AuraAbilitySystemComponent();

	void AbilityActorInfoSet();

	FMy_EffectAssetTags EffectAssetTags;
protected:

	//OnGameplayEffectAppliedDelegateToSelf 委托的回调函数,需要进行绑定
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
