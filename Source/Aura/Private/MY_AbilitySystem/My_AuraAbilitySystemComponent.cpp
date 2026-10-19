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
		// 开局被动天生"已装备"（它在跑），但没有 InputTag —— 槽位是空的，
		// 玩家得自己把它拖进某个被动槽才会长出 InputTag。
		// 这个状态 tag 是 ServerEquipAbility 的准入门槛（只收 Unlocked / Equipped）。
		AbilitySpec.DynamicAbilityTags.AddTag(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Equipped);
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


/* ==========================================================================================
 * 输入 → 能力 的三个入口：Pressed / Held / Released
 * ------------------------------------------------------------------------------------------
 * 【谁在什么时候调】（绑定见 My_AuraEnhancedInputComponent / AuraInputComponent::BindAbilityActions）
 *     ETriggerEvent::Started   → AbilityInputTagPressed     按下那一瞬间，只调【一次】
 *     ETriggerEvent::Triggered → AbilityInputTagHeld        按住期间，【每帧】调一次
 *     ETriggerEvent::Completed → AbilityInputTagReleased    松开那一瞬间，只调【一次】
 *
 * 【为什么"激活"写在 Held 里，而不是 Pressed 里】
 *     Pressed 只有一次机会。而技能激活【可能失败】，原因大多是【暂时】的：
 *       冷却还没走完 / 蓝不够 / 被 BlockedTags 挡住 / 正在播别的技能
 *     写在 Held 里 = 每帧重试一次 → "按住不放，冷却一结束就自动放出来"。
 *     `if (!IsActive())` 这个守卫保证技能进入激活后不会重复激活。
 *
 * 【底层就两个机制，一定要分清 —— 它们是两回事】
 *
 *   ① 标记 Spec.InputPressed（电平 / 状态："按键现在按着没有"）
 *        AbilitySpecInputPressed  → true
 *        AbilitySpecInputReleased → false
 *      谁读：WaitInputPress / WaitInputRelease 的 bTestAlreadyXxx 检测
 *            （对应蓝图节点上的 Test Already Pressed / Test Already Released 勾选框）
 *      ★ 注意：Task 只在【自己 Activate() 的那一刻】读一次这个标记（快照），之后不再读！
 *      解决："Task 开始等的时候，按键【已经】是按住状态了"
 *
 *   ② 事件 InvokeReplicatedEvent（边沿 / 一次性："刚刚按/松了一下"）
 *        把 InputPressed / InputReleased 广播给注册了委托的 AbilityTask
 *        （AbilityTask_WaitInputPress / _WaitInputRelease 内部监听的就是它）
 *      解决："Task 开始等【之后】，按键才按/松"
 *
 * 【Task 内部的两条路】（看 AbilityTask_WaitInputPress::Activate 即可确认）
 *     Task 创建时 → 勾了 bTestAlreadyXxx → 先读一次 ① 的标记
 *                      命中 → 立刻回调并 return（不注册委托）
 *                      没命中 → 继续下面
 *                 → 注册委托，等 ② 的事件来触发
 *
 * 【为什么 Held 里【不】广播事件】
 *     InvokeReplicatedEvent 是"一次性事件"语义，而 Held 每帧调用：
 *       - 每帧广播 → WaitInputPress 的委托每帧触发一次，逻辑全乱
 *       - 其中的 ServerSetReplicatedEvent 是 Reliable RPC，每帧发一个会把网络打爆
 *     "按住"这个【持续状态】已经由 ① 的标记表达了，不需要重复发事件。
 *
 * 【Pressed 里那句广播到底什么时候有用】
 *     首次按下时（Pressed → 下一帧 Held 才激活），IsActive() 为 false，广播【不执行】——
 *     这是正常的，因为技能激活时按键已经按住，靠 ① 的标记就够了（引擎头文件原话：
 *     "Will return 0 if input was already down"）。
 *     它真正生效的场景是【按下发生在技能激活之后】：
 *       - 技能被别的方式激活（被动 / 别的技能 / GameplayEvent）之后，才等玩家按一下
 *       - 技能还活着时玩家松开再按（二段输入）
 *     这两种情况下 Held 帮不上忙（它从不广播），只有 Pressed 这一条路能通知到 Task。
 *
 * 【为什么三个函数开头都有一把 FScopedAbilityListLock】
 *     GetActivatableAbilities() 返回的是 ActivatableAbilities.Items 的【引用】，不是拷贝。
 *     循环体里调 TryActivateAbility / EndAbility 可能让某个能力结束并被移出这个数组，
 *     数组一旦 RemoveAtSwap，正在遍历的引用就失效了。
 *     这把锁把"增删能力"改成【排队延后】：持锁期间 GiveAbility / ClearAbility 只把请求塞进
 *     AbilityPendingAdds / AbilityPendingRemoves，等锁析构（函数结束）时才真正改数组。
 *     ⚠️ 它不是线程锁，只是"延迟增删"的 RAII 计数器，可以嵌套。
 * ========================================================================================== */

/* 【按下瞬间，只调一次】
 * 职责：① 记下"按键按住了"（标记 = true）
 *       ② 如果技能【已经在跑】，把"又按了一次"这个事件广播出去 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 用 HasTagExact 精确匹配：DynamicAbilityTags 是技能被赋予时打到 Spec 上的输入 Tag
		// （同一个 InputTag 可能挂着多个技能，所以这里是循环处理全部匹配项）
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 引擎内部做【两件事】（见 AbilitySystemComponent_Abilities.cpp:2575-2594）：
			//   ① Spec.InputPressed = true  —— 标记，给 WaitInputXxx 的 bTestAlreadyXxx 读
			//   ② 若技能已激活 → 转发 InputPressed() 钩子给技能实例
			//      （UGameplayAbility::InputPressed 默认是【空实现】，留给 C++ 子类 override；
			//        引擎自己的 UGameplayAbility_CharacterJump 就 override 了 InputReleased
			//        —— 松开跳跃键就 CancelAbility，实现"可变高度跳跃"）
			AbilitySpecInputPressed(AbilitySpec);

			if (AbilitySpec.IsActive())
			{
				// 只有"技能已经在跑"时才广播：告诉正在等输入的 AbilityTask"玩家又按了一次"。
				// 首次按下时技能还没激活，走不到这里 —— 那一次靠上面那句【标记】兜住。
				// 引擎原注释：This is not replicated here. If someone is listening,
				//             they may replicate the InputPressed event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}


/* 【按住期间，每帧调一次】
 * 职责：① 每帧同步"按键按住了"（标记 = true）
 *       ② 每帧把"按住中"转发给技能实例的 InputPressed() 钩子（技能激活之后才开始真正转发）
 *       ③ 技能没激活就尝试激活 —— 失败不用管，下一帧自动再试
 * 注意：这里【不广播事件】—— 原因见上方说明 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// ① 标记（其实 Pressed 那一帧已经设过了，这句是"每帧维持一次状态"）
			//    技能激活之后，它还会顺带把 InputPressed() 钩子转发给技能实例 ——
			//    蓄力/持续型技能就是靠这个拿到"每帧按住"的信号。
			//    （本项目目前没有任何 C++ 技能 override 这个钩子，所以这部分实际上是空转）
			AbilitySpecInputPressed(AbilitySpec);

			// ② 没激活就试着激活。失败不需要处理：
			//    冷却中 / 蓝不够 / 被 Tag 挡 —— 下一帧会自动重试，冷却一结束就放出来。
			//    ★ 这就是"激活写在 Held 而不是 Pressed"的意义所在。
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

/* 【松开瞬间，只调一次】—— ★ 把标记设回 false 的是这里，不是 Held
 * 职责：① 标记 = false
 *       ② 转发 InputReleased() 钩子给技能实例
 *       ③ 广播 InputReleased 事件 —— 按住型技能（如电击）"松手生效"全靠它 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 注意比 Pressed/Held 多了一个 IsActive() 条件：
		//   只有"技能还活着"才有必要告诉它"松手了"。
		//   瞬发技能（如火球）松手时早已结束，整段都不会进来 —— 标记也不用改，
		//   反正下次按下会重新设成 true。
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			// ① 标记 = false（给 WaitInputXxx 的 bTestAlreadyXxx 用）
			//    ② 同时转发 InputReleased() 钩子给技能实例（C++ override 用）
			//    （注：这个函数本身【不广播任何委托】，广播在下面那句）
			AbilitySpecInputReleased(AbilitySpec);

			// ③ 广播"松开"事件 —— 这是 WaitInputRelease 的【唯一】触发途径。
			//    按住型技能（电击等）松手就生效，靠的就是这一句。
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
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

FGameplayTag UMy_AuraAbilitySystemComponent::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* Spec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromAbilitySpec(*Spec);
	}
	return FGameplayTag();
}

/* 这个技能占着【指定的】槽位吗（精确匹配：My_InputTag.1 不会命中 My_InputTag） */
bool UMy_AuraAbilitySystemComponent::AbilityHasSlot(const FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	return AbilitySpec.DynamicAbilityTags.HasTagExact(Slot);
}

/* 这个技能占着【任意】槽位吗（= 已装备）。
 * 直接复用"取 InputTag"的逻辑，不用再写一遍前缀匹配。 */
bool UMy_AuraAbilitySystemComponent::AbilityHasAnySlot(const FGameplayAbilitySpec& AbilitySpec)
{
	return GetInputTagFromAbilitySpec(AbilitySpec).IsValid();
}

/* 这个槽位被谁占着？没人占返回 nullptr。
 * ⚠️ 返回的是 ActivatableAbilities 内部的指针，函数返回时遍历锁已释放，
 *    调用方拿到后要尽快用完，不要跨多次遍历持有。 */
FGameplayAbilitySpec* UMy_AuraAbilitySystemComponent::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(Spec, Slot))
		{
			return &Spec;
		}
	}
	return nullptr;
}

bool UMy_AuraAbilitySystemComponent::SlotIsEmpty(const FGameplayTag& Slot)
{
	return GetSpecWithSlot(Slot) == nullptr;
}

/* 这个技能是被动吗？判定依据是 AbilityInfo 里配的 AbilityType —— 加新被动不用改这里 */
bool UMy_AuraAbilitySystemComponent::IsPassiveAbility(const FGameplayAbilitySpec& AbilitySpec) const
{
	const UMy_AbilityInfo* AbilityInfo = UMy_AuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (AbilityInfo == nullptr) return false;

	const FGameplayTag AbilityTag = GetAbilityTagFromAbilitySpec(AbilitySpec);
	if (!AbilityTag.IsValid()) return false;

	return AbilityInfo->FindAbilityInfoFromTag(AbilityTag).AbilityType
	                  .MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_Abilities_Type_Passive);
}

/* 把技能换到指定槽位：先清旧槽，再写新槽，最后标脏。
 * MarkAbilitySpecDirty 标的是整个 Item，会把所有字段一起同步，所以只需调一次。 */
void UMy_AuraAbilitySystemComponent::AssignSlotToAbility(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& Slot)
{
	ClearSlot(&AbilitySpec);
	AbilitySpec.DynamicAbilityTags.AddTag(Slot);
	MarkAbilitySpecDirty(AbilitySpec);
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
	const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();

	// ① 找到技能，校验状态：只有 Unlocked（已解锁）/ Equipped（已装备）能装
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	if (AbilitySpec == nullptr) return;

	const FGameplayTag StatusTag = GetStatusTagFromAbilitySpec(*AbilitySpec);
	const bool bCanEquip = StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Unlocked)
		|| StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Equipped);
	if (!bCanEquip) return;

	// 先把后面要用的信息取出来：激活能力可能改动能力列表，之后指针不保证有效
	const FGameplayAbilitySpecHandle Handle = AbilitySpec->Handle;
	const bool bPassive = IsPassiveAbility(*AbilitySpec);
	const FGameplayTag PreSlot = GetInputTagFromAbilitySpec(*AbilitySpec);

	// ② 点的就是它现在待的槽 → 数据不用改，回执让 UI 收尾
	if (PreSlot.MatchesTagExact(Slot))
	{
		ClientEquipAbility(AbilityTag, StatusTag, Slot, PreSlot);
		return;
	}

	// ③ 腾空目标槽：原来占着的不论主动被动，一律卸下
	//    旧被动还要额外通知它 EndAbility，否则光环特效 / 持续 GE 会一直挂着
	if (FGameplayAbilitySpec* Occupant = GetSpecWithSlot(Slot))
	{
		if (IsPassiveAbility(*Occupant))
		{
			//特效委托
			MulticastActivatePassiveEffect(GetAbilityTagFromAbilitySpec(*Occupant), false);
			OnDeactivatePassiveAbility.Broadcast(GetAbilityTagFromAbilitySpec(*Occupant));
		}
		ClearSlot(Occupant);
	}

	// ④ 写新槽位（内部先清旧槽再 AddTag，并标脏复制）
	AssignSlotToAbility(*AbilitySpec, Slot);

	// ⑤ 状态推进：Unlocked → Equipped（纯换键时状态不变）
	if (StatusTag.MatchesTagExact(GameplayTags.My_Abilities_Status_Unlocked))
	{
		AbilitySpec->DynamicAbilityTags.RemoveTag(GameplayTags.My_Abilities_Status_Unlocked);
		AbilitySpec->DynamicAbilityTags.AddTag(GameplayTags.My_Abilities_Status_Equipped);
		MarkAbilitySpecDirty(*AbilitySpec);
	}

	// ⑥ 被动技能装上就激活；主动技能等玩家按键，这里不碰
	//    ⚠️ 这里必须是 true（开启特效）。写成 false 客户端会走 Deactivate，
	//       表现就是"技能装上了、状态也对、但光环永远不亮"。
	//    另外：开局直授的被动此时 IsActive() 已经是 true，整个 if 会被跳过，
	//    它们靠 UMy_PassiveNiagaraComponent::ActivateIfEquipped 在 BeginPlay 兜底。
	if (bPassive && !AbilitySpec->IsActive())
	{
		MulticastActivatePassiveEffect(AbilityTag, true);
		TryActivateAbility(Handle);
	}

	// ⑦ 回执：带 PreSlot，UI 才能精确地"清旧槽 + 填新槽"
	ClientEquipAbility(AbilityTag, GameplayTags.My_Abilities_Status_Equipped, Slot, PreSlot);
}

/* 装备回执（服务器 → 触发操作的客户端）
 * 这里只做一个中转广播：把"谁、变成什么状态、进了哪个槽、离开哪个槽"交给 WidgetController 去分发给 UI。
 * 分层意义：ASC 不认识 UI，只广播数据；WidgetController 才决定怎么更新界面。 */
void UMy_AuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	OnAbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreSlot);
}

/* 【针对 Ability】清掉这一个技能占的槽位（只删它自己的 InputTag）
* ⚠️ DynamicAbilityTags 属于 FFastArraySerializerItem，改完必须 MarkAbilitySpecDirty，
*    否则这次变化不会复制到客户端 —— 客户端 UI 会以为它还占着那个槽。 */
void UMy_AuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* AbilitySpec)
{
	if (AbilitySpec == nullptr) return;

	const FGameplayTag SlotTag = GetInputTagFromAbilitySpec(*AbilitySpec);
	if (!SlotTag.IsValid()) return;

	AbilitySpec->DynamicAbilityTags.RemoveTag(SlotTag);
	MarkAbilitySpecDirty(*AbilitySpec);
}

void UMy_AuraAbilitySystemComponent::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTags, bool bActivate)
{
	OnActivatePassiveAbility.Broadcast(AbilityTags, bActivate);
}

/* 【针对槽位】清掉占着这个槽的所有技能（内部转调 ClearSlot） */
void UMy_AuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopedLock(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(AbilitySpec, Slot))
		{
			ClearSlot(&AbilitySpec);
		}
	}
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
