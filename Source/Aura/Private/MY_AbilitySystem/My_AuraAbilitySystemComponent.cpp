// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "Aura/AuraLogChannels.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "MY_AbilitySystem/Data/My_AbilityInfo.h"
#include "My_Interraction/My_PlayerInterface.h"

UMy_AuraAbilitySystemComponent::UMy_AuraAbilitySystemComponent()
{
	SetIsReplicated(true);
}

//在Character_Base的SetAbilityActorInfo()函数中调用
void UMy_AuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//-------一旦Actorinfo设置好,就可以对ASC的委托进行绑定-----------------
	// 该委托只在服务器调用,客户端不使用,因此使用RPC方法解决问题
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMy_AuraAbilitySystemComponent::ClientEffectApplied);
}

//把"服务器上施加了 GE"这件事通知给客户端，让 UI 显示效果消息（比如拾取药水、获得增益）
void UMy_AuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                        const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	//使用委托进行TagContainer的广播,这样WidgetController就可以接受到
	//这样My_ASC不知道WidgetController,而WidgetController知道ASC
	EffectAssetTags.Broadcast(TagContainer);
}

/*
 * 在角色初始化时,给能力添加Tag
 */
void UMy_AuraAbilitySystemComponent::AddCharacterAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbility)
{
	for (auto& Ability : StartupAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);

		if (const UMy_AuraGameplayAbilityBase* AuraAbility = Cast<UMy_AuraGameplayAbilityBase>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}

	//处理技能UI显示的旗子标记
	bStartupAbilityGiven = true;
	/* 角色添加能力的时候，可以给这个能力设置UI，和AbilityInfo对比*/
	OnAbilityGiven.Broadcast();
}

void UMy_AuraAbilitySystemComponent::AddCharacterPassiveAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbility)
{
	for (auto& Ability : StartupPassiveAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}


//处理客户端不显示Ability的UI问题
void UMy_AuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilityGiven)
	{
		bStartupAbilityGiven = true;
		OnAbilityGiven.Broadcast();
	}
}


/*
 * 当PlayerController 按下/放开 按键会激活下面的函数
 * 根据inputTag激活对应的能力
 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UMy_AuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UMy_AuraAbilitySystemComponent::ForEachAbility(const FMy_ForEachAbilitySignature& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetAbilityTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_Abilities")))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_InputTag")))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetStatusTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_Abilities.Status")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetStatusTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFromAbilitySpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromAbilitySpec(*Spec);
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UMy_AuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : Spec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}


/*
 * AttributeMenu部分
 */
void UMy_AuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UMy_PlayerInterface>())
	{
		//有属性点
		if (IMy_PlayerInterface::Execute_GetAttributePointFormPlayerState(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UMy_AuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag AttributeTag)
{
	if (IMy_PlayerInterface::Execute_GetAttributePointFormPlayerState(GetAvatarActor()) <= 0) return;
	FGameplayEventData EventData;
	EventData.EventTag = AttributeTag;
	EventData.EventMagnitude = 1.f;
	//使用SendEvent的方式修改Attribute,因为PassiveAbility可以监听AttributeTag的Event
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, EventData);
	//属性点-1
	IMy_PlayerInterface::Execute_AddToAttributePoint(GetAvatarActor(), -1);
}

/*
 * SpellMenu部分
 */
void UMy_AuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UMy_AbilityInfo* AbilityInfo = UMy_AuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityInfo) return;
	// 等级提升时，更新Abilityinfo中所有能力的Status
	for (const FMy_AuraAbilityInfo& info : AbilityInfo->AbilityInformation)
	{
		if (!info.AbilityTag.IsValid() || !info.AbilityClass || Level < info.LevelUpRequirement) continue;
		if (GetSpecFromAbilityTag(info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(info.AbilityClass, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			ClientUpdateAbilityStatus(info.AbilityTag, FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Eligible, 1);
		}
	}
}

void UMy_AuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChanged.Broadcast(AbilityTags, StatusTag, AbilityLevel);
}

void UMy_AuraAbilitySystemComponent::ServerSpendSpellPoints_Implementation(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		// 减少SpellPoint
		if (GetAvatarActor()->Implements<UMy_PlayerInterface>())
		{
			IMy_PlayerInterface::Execute_AddToSpellPoint(GetAvatarActor(), -1);
		}

		// 更改Ability状态
		FGameplayTag StatusTag = GetStatusTagFromAbilitySpec(*Spec);
		FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();
		if (StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Eligible))
		{
			Spec->DynamicAbilityTags.RemoveTag(GameplayTags.My_Abilities_Status_Eligible);
			Spec->DynamicAbilityTags.AddTag(GameplayTags.My_Abilities_Status_Unlocked);
			StatusTag = GameplayTags.My_Abilities_Status_Unlocked;
		}
		else if (StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Equipped) || StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Unlocked))
		{
			Spec->Level += 1;
		}

		// 通知客户端AbilityStatus已经改变
		ClientUpdateAbilityStatus(AbilityTag, StatusTag, Spec->Level);
		MarkAbilitySpecDirty(*Spec);
	}
}


/* ============================================================
 * 装备技能（Server RPC）—— 服务器权威端
 * ------------------------------------------------------------
 * 调用链：UI 点装备槽 → SpellMenuWidgetController::EquipSpellRowGlobePressed
 *            → ServerEquipAbility(AbilityTag, Slot)  ← 本函数
 *            → ClientEquipAbility(...) 回执给触发者
 *            → SpellMenuWidgetController::OnAbilityEquipped 广播给 UI
 *
 * 参数：AbilityTag = 要装备哪个技能；Slot = 装备到哪个槽（= 一个 InputTag）
 *
 * ⚠️ 为什么必须走 Server RPC：DynamicAbilityTags 的权威副本在服务器，
 *    客户端本地改会被下一次属性复制覆盖掉（改了不算数）。
 * ============================================================ */
void UMy_AuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();

		// ① 记下"旧槽位"——后面要原样带回给客户端，UI 靠它清掉旧槽的技能球
		//    （属性复制给不了这个"变化过程"，所以必须由 RPC 参数携带）
		const FGameplayTag& PreSlot = GetInputTagFromAbilitySpec(*AbilitySpec);

		// ② 校验状态：只有 Unlocked / Equipped 才允许装备
		//    Locked（没解锁）、Eligible（能加点但还没解锁）都不给装
		const FGameplayTag& StatusTag = GetStatusTagFromAbilitySpec(*AbilitySpec);
		const bool bStatusValid = StatusTag == GameplayTags.My_Abilities_Status_Equipped || StatusTag == GameplayTags.My_Abilities_Status_Unlocked;
		if (bStatusValid)
		{
			// ③ 清掉目标槽原来的占用者（一个槽只能装一个技能）
			//    注意：这一步也会把 AbilitySpec 自己清掉（如果它本来就占着这个槽）
			ClearAbilitiesOfSlot(Slot);

			// ④ 清掉这个技能原来的槽（换键 = 先脱离旧槽）
			//    ClearSlot 内部会 MarkAbilitySpecDirty，这里再调一次是双保险
			ClearSlot(AbilitySpec);

			// ⑤ 把技能放进新槽（AddTag 就是"装备到某个键位"的本体）
			AbilitySpec->DynamicAbilityTags.AddTag(Slot);

			// ⑥ 状态推进：Unlocked（已解锁未装备）→ Equipped（已装备）
			//    已经是 Equipped 的（纯换键）不用改状态
			if (StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Unlocked))
			{
				AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.My_Abilities_Status_Unlocked);
				AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.My_Abilities_Status_Equipped);
			}

			// ⑦ 标脏 → 让 FastArraySerializer 把这次的 DynamicAbilityTags 变化复制给客户端
			//    （DynamicAbilityTags 本身是复制的，但 Item 的"脏标记"要手动置位才会进增量同步）
			MarkAbilitySpecDirty(*AbilitySpec);
		}

		// ⑧ 回执给触发操作的客户端，带上 PreSlot 让 UI 能精确地"清旧槽 + 填新槽"
		ClientEquipAbility(AbilityTag, GameplayTags.My_Abilities_Status_Equipped, Slot, PreSlot);
	}
}

/* 装备回执（服务器 → 触发操作的客户端）
 * 这里只做一个中转广播：把"谁、变成什么状态、进了哪个槽、离开哪个槽"交给 WidgetController 去分发给 UI。
 * 分层意义：ASC 不认识 UI，只广播数据；WidgetController 才决定怎么更新界面。 */
void UMy_AuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	OnAbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreSlot);
}

/* 【针对 Ability】清掉这一个技能占的槽
 * 做法：先读出它当前的 InputTag，再把这个 Tag 删掉。
 * 结果：技能变成"没有槽位"（未装备状态）。
 * ⚠️ FGameplayAbilitySpec 是 FFastArraySerializerItem，改了里面的数据必须 MarkItemDirty，
 *    否则增量复制不会带上这次变化。 */
void UMy_AuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* AbilitySpec)
{
	const FGameplayTag& SlotTag = GetInputTagFromAbilitySpec(*AbilitySpec);
	AbilitySpec->DynamicAbilityTags.RemoveTag(SlotTag);
	MarkAbilitySpecDirty(*AbilitySpec);
}

/* 【针对槽位】清掉占着这个槽的所有技能
 * 与 ClearSlot 的区别（一个以"技能"为出发点，一个以"槽"为出发点）：
 *   ClearSlot(Spec)       : 清"这个技能"的槽
 *   ClearAbilitiesOfSlot  : 遍历所有技能，谁占着这个槽就清谁（内部还是调 ClearSlot）
 * 用途：装备时清掉目标槽的原占用者。
 * FScopedAbilityListLock 是遍历 ActivatableAbilities 时必须加的锁（防止遍历期间列表被改）。 */
void UMy_AuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&AbilitySpec, Slot))
		{
			ClearSlot(&AbilitySpec);
		}
	}
}

/* 【精确判断】这个技能占着【指定的这个 Slot】吗
 * 语义 = "它现在装备在你问的这个键位上吗"（HasTagExact 精确匹配）。
 * 与"这个技能装备了没"不是一回事：
 *   - 本函数问"占着某个具体槽吗"   → 需要传入具体 SlotTag
 *   - "装备了没"应该判断"有没有任意 InputTag 前缀的 Tag"（教程里叫 AbilityHasAnySlot） */
bool UMy_AuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot)
{
	for (FGameplayTag Tag : AbilitySpec->DynamicAbilityTags)
	{
		if (Tag.MatchesTagExact(Slot))
		{
			return true;
		}
	}
	return false;
}

bool UMy_AuraAbilitySystemComponent::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel, UMy_AbilityInfo* AbilityInfo, FString& OutDescription, FString& OutNextLevelDescription)
{
	// ★ 方案A：描述按 StatusTag 决定，不依赖"spec 此刻是否就位"（host/客户端复制时序都稳）
	const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();

	// 状态不是 Locked → 技能已授予，取真实技能描述
	if (!StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Locked))
	{
		int32 Level = AbilityLevel > 0 ? AbilityLevel : 1;

		// 优先用 spec（已就位时等级最准，例如点击/花点升级后）
		if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
		{
			Level = AbilitySpec->Level;
			if (UMy_AuraGameplayAbilityBase* AuraAbility = Cast<UMy_AuraGameplayAbilityBase>(AbilitySpec->Ability))
			{
				OutDescription = AuraAbility->GetDescription(Level);
				OutNextLevelDescription = AuraAbility->GetNextLevelDescription(Level + 1);
				return true;
			}
		}

		// spec 还没就位（host/复制的空窗期）→ 用 AbilityInfo 的 AbilityClass 的 CDO + 传入等级取描述
		// （AbilitySpec->Ability 本来就是该类的 CDO，所以结果一致）
		if (AbilityInfo)
		{
			const FMy_AuraAbilityInfo& Info = AbilityInfo->FindAbilityInfoFromTag(AbilityTag);
			if (Info.AbilityClass)
			{
				if (UMy_AuraGameplayAbilityBase* AbilityCDO = Info.AbilityClass->GetDefaultObject<UMy_AuraGameplayAbilityBase>())
				{
					OutDescription = AbilityCDO->GetDescription(Level);
					OutNextLevelDescription = AbilityCDO->GetNextLevelDescription(Level + 1);
					return true;
				}
			}
		}

		OutDescription = FString();
		OutNextLevelDescription = FString();
		return false;
	}

	// Locked：显示"需等级X解锁"
	if (AbilityInfo)
	{
		if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_Abilities_None))
		{
			OutDescription = FString();
		}
		else
		{
			OutDescription = UMy_AuraGameplayAbilityBase::GetLockDescription(AbilityInfo->FindAbilityInfoFromTag(AbilityTag).LevelUpRequirement);
		}
	}
	OutNextLevelDescription = FString();
	return false;
}
