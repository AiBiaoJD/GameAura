// Copyright ABiao


#include "My_UI/WidgetController/My_SpellMenuWidgetController.h"

#include "My_AuraGamePlayTags_Singleton.h"


void UMy_SpellMenuWidgetController::BroadcastInitiaValues()
{
	BroadcastAbilityInfo();
	OnPlayerSpellPointChanged.Broadcast(GetAuraPS()->GetSpellPoint());
}

void UMy_SpellMenuWidgetController::BindCallbacksToDependencies()
{
	// ★★★ 为什么这套"缓存 + 双回调重算"：
	// ASC 的 StatusTag 和 PS 的 SpellPoint 是两条独立的网络复制通道，
	// 服务器 → 客户端的到达时间/顺序不确定（可能 Status 先到、点数后到，或反过来）。
	// 而"按钮是否可用 = f(技能状态, 法术点数)"需要同时知道两个输入，所以：
	//   - 状态：不能随时现读（Locked 无 Spec 可读，Spec 复制有延迟）→ 缓存到 SelectedAbility.StatusTag
	//   - 点数：可直接现读 GetAuraPS()->GetSpellPoint()（复制属性，随时是最新值）→ 不需要缓存
	// 任一回调触发 → 用"这一边的新值 + 另一边的最新已知值"重算按钮 → 无论先后顺序，最终一定正确。

	// ── 技能状态变化（来自 ASC 复制）──
	GetAuraASC()->OnAbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag, int32 AbilityLevel)
	{
		// 只有"当前选中的技能"状态变了，才需要重算按钮
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTags))
		{
			// 1. 更新缓存的状态（回调送来的就是权威值）
			SelectedAbility.StatusTag = StatusTag;

			// 2. 用【新状态 + 现读点数】重算按钮
			//    （点数此刻可能还是旧值，没关系——点数回调到达后还会再算一次，保证最终正确）
			bool bSpendPointsButtonEnabled = false;
			bool bEquipButtonEnabled = false;
			My_ShouldEnableButton(StatusTag, GetAuraPS()->GetSpellPoint(), bSpendPointsButtonEnabled, bEquipButtonEnabled);

			// 3. 广播给 UI 更新按钮状态
			OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled);
		}

		// 无论是否选中，都刷新技能描述信息（等级/伤害等）
		if (AbilityDA)
		{
			FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTags);
			info.StatusTag = StatusTag;
			OnAbilityInfo.Broadcast(info);
		}
	});

	// ── 法术点数变化（来自 PS 复制）──
	if (GetAuraPS())
	{
		GetAuraPS()->OnSpellPointChanged.AddLambda([this](int32 NewSpellPoint)
		{
			// 1. 广播新的点数给 UI（技能球上的数字）
			OnPlayerSpellPointChanged.Broadcast(NewSpellPoint);

			// 2. 用【新点数 + 缓存状态】重算按钮
			//    （状态此刻可能还没复制到，用缓存值——状态回调到达后还会再算一次，保证最终正确）
			bool bSpendPointsButtonEnabled = false;
			bool bEquipButtonEnabled = false;
			My_ShouldEnableButton(SelectedAbility.StatusTag, NewSpellPoint, bSpendPointsButtonEnabled, bEquipButtonEnabled);

			// 3. 广播给 UI 更新按钮状态
			OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled);
		});
	}
}

void UMy_SpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	// 点击技能球：算出这个技能的当前状态，缓存起来，并广播一次按钮状态
	const int32 SpellPoint = GetAuraPS()->GetSpellPoint();

	FGameplayTag AbilityStatus;
	const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();

	// 三种情况都视为"锁定"：tag 非法 / tag 是 None 占位 / 该技能没被授予（没有 Spec）
	const bool bTagValid = AbilityTag.IsValid(); //AbilityTag合法
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.My_Abilities_None); //AbilityTag为空
	const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = Spec != nullptr; //Ability存在

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.My_Abilities_Status_Locked;
	}
	else
	{
		// 已授予：从 Spec 的动态标签里读出状态
		AbilityStatus = GetAuraASC()->GetStatusTagFromAbilitySpec(*Spec);
	}

	// 缓存当前选中的技能 + 状态（供后续回调重算按钮用）
	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.StatusTag = AbilityStatus;

	// 首次点击：直接算一次按钮状态并广播
	bool bSpendPointsButtonEnabled = false;
	bool bEquipButtonEnabled = false;
	My_ShouldEnableButton(AbilityStatus, SpellPoint, bSpendPointsButtonEnabled, bEquipButtonEnabled);
	OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled);
}

// 核心规则：什么状态下哪个按钮可用
//   Locked    → 花点/装备 都不可用
//   Eligible  → 只有花点可用（解锁技能），装备不可用
//   Equipped  → 装备可用；有点数时花点也可用
//   Unlocked  → 装备可用；有点数时花点也可用
void UMy_SpellMenuWidgetController::My_ShouldEnableButton(FGameplayTag AbilityStatus, const int32 SpellPoint, bool& bSpendPointsButtonEnabled, bool& bEquipButtonEnabled)
{
	const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();
	if (AbilityStatus == GameplayTags.My_Abilities_Status_Locked)
	{
		bEquipButtonEnabled = false;
		bSpendPointsButtonEnabled = false;
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Eligible)
	{
		bEquipButtonEnabled = false;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Equipped)
	{
		bEquipButtonEnabled = true;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Unlocked)
	{
		bEquipButtonEnabled = true;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
}

void UMy_SpellMenuWidgetController::SpendPointsButtonPressed()
{
	GetAuraASC()->ServerSpendSpellPoints(SelectedAbility.AbilityTag);
}
