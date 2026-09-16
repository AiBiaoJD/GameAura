// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "My_AuraAbilitySystemComponent.generated.h"

class UMy_AbilityInfo;

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_EffectAssetTagsSignature, const FGameplayTagContainer& /*AssetTags*/)
DECLARE_MULTICAST_DELEGATE(FMy_AbilityGivenSignature);
DECLARE_DELEGATE_OneParam(FMy_ForEachAbilitySignature, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FMy_AbilityStatusChangedSignature, const FGameplayTag& /*AbilityTags*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLeve*/)
DECLARE_MULTICAST_DELEGATE_FourParams(FMy_AbilityEquippedSignature, const FGameplayTag& /*AbilityTags*/, const FGameplayTag& /*StatusTags*/, const FGameplayTag& /*Slot*/, const FGameplayTag& /*PreSlot*/)

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
	FMy_AbilityEquippedSignature OnAbilityEquipped; //处理装配技能后,发送给widgetController的相关数据
	
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
	FGameplayTag GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);

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

	UFUNCTION(Server, Reliable)
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	/* 装备完成回执（服务器 → 触发操作的客户端）
	 * 为什么要额外发这条 RPC，而不是只靠属性复制：
	 *   ① PreviousSlot 拿不到：复制只给"最终状态"，不给"变化过程"。
	 *      UI 需要清掉旧槽位的技能球，必须知道旧槽是哪个；靠复制得自己缓存+算 diff。
	 *   ② 时序：RPC 当帧/下一帧就到；属性复制要等 NetUpdateFrequency 周期（几十~上百 ms），UI 会顿一下。
	 *   ③ 广播量：这条 RPC 让 UI 只广播 2 条（旧槽清空 + 新槽装备）；靠复制只能全量重播。
	 * ⚠️ 但 ServerEquipAbility 是省不掉的（客户端无 Authority）；能省的只有这条 Client RPC，
	 *    且仅在"纯单机 / 你是 Listen Server Host"时可省。
	 */
	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot);

	/* ===== 装备槽位辅助函数 =====
	 * 术语说明：Slot（槽位）和 InputTag（输入标签）是【同一个 FGameplayTag】，
	 * 只是两个语境的命名约定：
	 *   - InputTag：强调"按哪个键"（AbilityInputTagHeld 用）
	 *   - Slot   ：强调"显示在哪个格子 / 能不能塞进去"（装备系统用）
	 * 换成 Slot 命名是为了可读性：AbilityHasSlot 比 AbilityHasInputTag 更能表达"占着格子没"。
	 */

	// 【针对 Ability】清掉这一个技能占的槽位（只删它自己的 InputTag）
	void ClearSlot(FGameplayAbilitySpec* AbilitySpec);
	// 【针对槽位】清掉占着这个槽的所有技能（遍历所有技能，内部转调 ClearSlot）
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);
	// 【精确判断】这个技能占着【指定的这个 Slot】吗（HasTagExact(Slot)）
	// ⚠️ 注意与"是否装备了"的区别：判断"装备了没"应该用"有没有任意 InputTag"的写法，
	//    本函数是"占着某个具体槽吗"，传入具体 SlotTag 才有意义。
	bool AbilityHasSlot(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot);


	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel, UMy_AbilityInfo* AbilityInfo, FString& OutDescription, FString& OutNextLevelDescription);

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
