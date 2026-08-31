// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "My_AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_EffectAssetTagsSignature, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FMy_AbilityGivenSignature);
DECLARE_DELEGATE_OneParam(FMy_ForEachAbilitySignature, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMy_AbilityStatusChangedSignature, const FGameplayTag& /*AbilityTags*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLeve*/)

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

	FMy_EffectAssetTagsSignature EffectAssetTags; //处理捡东西MessageUI
	FMy_AbilityGivenSignature OnAbilityGiven; //处理技能显示UI
	FMy_AbilityStatusChangedSignature OnAbilityStatusChanged; //处理等级提升技能变为Eligible

	// Character添加能力
	void AddCharacterAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbility);
	void AddCharacterPassiveAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbility);
	bool bStartupAbilityGiven = false;

	// PlayerController激活能力
	void AbilityInputTagHeld(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);

	/* Ability */
	// 一个委托激活所有能力的回调函数,传递Abilityinfo
	void ForEachAbility(const FMy_ForEachAbilitySignature& Delegate);
	static FGameplayTag GetAbilityTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec);

	// AttributeMenu中按钮点击提示属性点
	void UpgradeAttribute(const FGameplayTag& AttributeTag);
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag AttributeTag);

	// 更新SpellMenu中Ability的Status
	void UpdateAbilityStatuses(int32 Level);
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	// SpellMenu中Spend按钮触发
	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoints(const FGameplayTag& AbilityTag);

protected:
	virtual void OnRep_ActivateAbilities() override;
	/*
	 * OnGameplayEffectAppliedDelegateToSelf 委托是只在服务器调用回调函数
	 * 因此客户端不调用这个回调函数,采用RPC解决
	 * RPC:服务器调用客户端函数，客户端执行函数
	 */
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag, int32 AbilityLevel);
};
