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
class AURA_API UMy_AuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	UMy_AuraAbilitySystemComponent();

	void AbilityActorInfoSet();


	FMy_EffectAssetTags EffectAssetTags;

	// Character添加能力
	void AddCharacterAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbility);


	// PlayerController激活能力
	void AbilityInputTagHeld(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);

protected:

	/*
	 * OnGameplayEffectAppliedDelegateToSelf 委托是只在服务器调用回调函数
	 * 因此客户端不调用这个回调函数,采用RPC解决
	 * RPC:服务器调用客户端函数，客户端执行函数
	 */
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
