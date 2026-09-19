// Copyright ABiao


#include "My_UI/WidgetController/My_SpellMenuWidgetController.h"
#include "AuraGameplayTags.h"
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
	GetAuraASC()->OnAbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
	{
		// 只有"当前选中的技能"状态变了，才需要重算按钮
		if (SelectedAbility.AbilityTag.MatchesTagExact(AbilityTag))
		{
			// 1. 更新缓存的状态（回调送来的就是权威值）
			SelectedAbility.StatusTag = StatusTag;

			// 2. 用【新状态 + 现读点数】重算按钮
			//    （点数此刻可能还是旧值，没关系??点数回调到达后还会再算一次，保证最终正确）
			bool bSpendPointsButtonEnabled = false;
			bool bEquipButtonEnabled = false;
			My_ShouldEnableButton(StatusTag, GetAuraPS()->GetSpellPoint(), bSpendPointsButtonEnabled, bEquipButtonEnabled);

			// 3. 广播给 UI 更新按钮状态
			FString DescriptionString;
			FString NexeLevelDescriptionString;
			GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, StatusTag, AbilityLevel, AbilityDA, DescriptionString, NexeLevelDescriptionString);
			OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, DescriptionString, NexeLevelDescriptionString);
		}

		// 无论是否选中，都刷新技能描述信息（等级/伤害等）
		if (AbilityDA)
		{
			FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTag);
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
			//    （状态此刻可能还没复制到，用缓存值??状态回调到达后还会再算一次，保证最终正确）
			bool bSpendPointsButtonEnabled = false;
			bool bEquipButtonEnabled = false;
			My_ShouldEnableButton(SelectedAbility.StatusTag, NewSpellPoint, bSpendPointsButtonEnabled, bEquipButtonEnabled);

			// 3. 广播给 UI 更新按钮状态
			FString DescriptionString;
			FString NexeLevelDescriptionString;
			GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, SelectedAbility.StatusTag, 0, AbilityDA, DescriptionString, NexeLevelDescriptionString);
			OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, DescriptionString, NexeLevelDescriptionString);
		});
	}


	GetAuraASC()->OnAbilityEquipped.AddUObject(this, &UMy_SpellMenuWidgetController::OnAbilityEquipped);
}

void UMy_SpellMenuWidgetController::GlobeDeselect()
{
	if (bWaitForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityDA->FindAbilityInfoFromTag(SelectedAbility.AbilityTag).AbilityType;
		OnStopWaitForEquipSelection.Broadcast(SelectedAbilityType);
		bWaitForEquipSelection = false;
	}

	SelectedAbility.AbilityTag = FMy_AuraGameplayTags::GetInstance().My_Abilities_None;
	SelectedAbility.StatusTag = FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Locked;
	OnSpellGlobeSelect.Broadcast(false, false, FString(), FString());
}

void UMy_SpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		FGameplayTag SelectedAbilityType = AbilityDA->FindAbilityInfoFromTag(SelectedAbility.AbilityTag).AbilityType;
		OnStopWaitForEquipSelection.Broadcast(SelectedAbilityType);
		bWaitForEquipSelection = false;
	}


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
	FString DescriptionString;
	FString NexeLevelDescriptionString;
	GetAuraASC()->GetDescriptionByAbilityTag(AbilityTag, AbilityStatus, 0, AbilityDA, DescriptionString, NexeLevelDescriptionString);
	OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled, DescriptionString, NexeLevelDescriptionString);
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


/* 点「装备」按钮 —— 进入"等待选择装备槽"模式
 * ------------------------------------------------------------
 * 这一步【只改 UI 状态，不装备任何东西】。真正的装备要等玩家点了具体槽位。
 *
 * 三段作用：
 *   ① 取技能类型（Offensive / Passive）→ 广播给 UI，让 UI 高亮对应那一排槽位
 *      （主动技能不能装进被动槽，所以要先让 UI 知道该高亮哪一排）
 *   ② 置 bWaitForEquipSelection = true  → 进入"选槽模式"（状态机开关）
 *      后续点球/点槽/取消，都要靠这个标志判断"现在是在选槽还是在普通浏览"
 *   ③ 记录当前槽位到 SelectedSlot —— 目前【只是存下来，没有读取点】
 *      用途预留：以后若要做"点当前槽 = 取消装备"或"高亮当前装备槽"时需要它。
 *      之所以在这里现读而不是用 SelectedAbility.Status 缓存，是因为状态是易变数据，
 *      要在用它的那一刻从 ASC 取真值（判断"装备了没"和"取槽位"要用同一份来源）。
 */
void UMy_SpellMenuWidgetController::EquippedButtonPressed()
{
	// ① 广播给 UI：进入选槽模式 + 高亮该类型那一排
	const FGameplayTag AbilityType = AbilityDA->FindAbilityInfoFromTag(SelectedAbility.AbilityTag).AbilityType;
	OnWaitForEquipSelection.Broadcast(AbilityType);

	// ② 状态机开关（T字路口：后面所有操作都要看它）
	bWaitForEquipSelection = true;

	// ③ 记录当前槽位（目前未使用，见函数头注释）
	const FGameplayTag SelectedStatusTag = GetAuraASC()->GetStatusTagFromAbilityTag(SelectedAbility.AbilityTag);
	if (SelectedStatusTag == FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Equipped)
	{
		SelectedSlot = GetAuraASC()->GetInputTagFromAbilityTag(SelectedAbility.AbilityTag);
	}
}

/* 点装备行里的某个槽位 —— 选槽模式的终点
 * 参数：SlotTag = 被点击的槽位标签（本质是一个 InputTag，如 My_InputTag.3）
 *       AbilityType = 这个槽位所属的类型（主动排 / 被动排），由 UI 在生成槽位时传进来
 *
 * 三重门禁（任一不过就直接 return）：
 *   ① 不在选槽模式      → 忽略（防止普通浏览时误触）
 *   ② 技能类型和槽不匹配 → 忽略（主动技能不能塞进被动槽，反之亦然）
 *   ③ 通过后 → 发 Server RPC 请求服务器执行装备
 *
 * ⚠️ 服务器端 ServerEquipAbility 还会再校验一次状态合法性（Locked/Eligible 不给装），
 *    所以客户端这两层判断只是"提前拦掉明显非法的操作"，不是安全边界。
 */
void UMy_SpellMenuWidgetController::EquipSpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& AbilityType)
{
	// ① 不在选槽模式 → 不是装备操作，忽略
	if (!bWaitForEquipSelection) return;

	// ② 类型校验：拿【当前选中技能】的类型和【被点槽位】的类型比对
	//    （注意这里是 SelectedAbility 的类型，不是 SlotTag 的）
	const FGameplayTag& SelectedAbilityType = AbilityDA->FindAbilityInfoFromTag(SelectedAbility.AbilityTag).AbilityType;
	if (!SelectedAbilityType.MatchesTagExact(AbilityType)) return;

	// ③ 请求服务器装备（客户端无 Authority，必须走 Server RPC）
	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag, SlotTag);
}

/* 装备完成的回调 —— 由 ASC 的 ClientEquipAbility RPC 触发（只在触发操作的那个客户端执行）
 * ------------------------------------------------------------
 * 这里做两件事：① 广播恰好两条 AbilityInfo 给 UI；② 收尾（结束选槽模式 + 播放取消动画）
 *
 * 参数：AbilityTag = 刚被装备的技能
 *       Status     = 它现在的新状态（Equipped）
 *       Slot       = 它进入的新槽位
 *       PreSlot    = 它离开的旧槽位（RPC 专门带过来，属性复制给不了这个信息）
 *
 * ★ 为什么是"两条"而不是调用 BroadcastAbilityInfo() 全量重播：
 *     全量重播要等属性复制到客户端、且拿不到"旧槽是哪个"；
 *     而这条 RPC 的参数里直接带着 Slot / PreSlot，所以能立刻、精确地广播两条。
 *     - 第 1 条：旧槽广播一条"空" → UI 把旧槽的技能球拿掉
 *     - 第 2 条：新槽广播真实技能 → UI 在新槽放上技能球
 *
 * ⚠️ UI 拿到 AbilityInfo 后，是用 info.InputTag 决定"球放在哪个槽"，
 *    所以"改了 InputTag → 球的显示位置变了"的真相是：
 *    服务器改了数据 → 这里广播新数据 → UI 照新数据重画（UI 从不自己决定球在哪）。
 */
void UMy_SpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreSlot)
{
	// 收尾：已经装备完了，退出选槽模式
	// （注意：这里直接置 false，没有走 OnStopWaitForEquipSelection 广播，动画靠函数末尾那次广播收）
	bWaitForEquipSelection = false;

	// ① 旧槽位：广播一条"空信息"（AbilityTag = None）告诉 UI「这个槽现在空了，把球拿走」
	//    StatusTag 用 Unlocked 而不是 Locked：技能本身还是"已解锁"的，只是不在这个槽了
	FMy_AuraAbilityInfo LastSlotInfo;
	LastSlotInfo.StatusTag = FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Unlocked;
	LastSlotInfo.InputTag = PreSlot;
	LastSlotInfo.AbilityTag = FMy_AuraGameplayTags::GetInstance().My_Abilities_None;
	OnAbilityInfo.Broadcast(LastSlotInfo);

	// ② 新槽位：广播真实技能信息 → UI 把它画到新槽
	//    从 AbilityDA 取出该技能的静态配置（描述、图标等），再覆盖上本次变化的两个动态值
	FMy_AuraAbilityInfo Info = AbilityDA->FindAbilityInfoFromTag(AbilityTag);
	Info.InputTag = Slot;
	Info.StatusTag = Status;
	OnAbilityInfo.Broadcast(Info);

	// ③ 结束"选槽模式"的视觉表现：播取消选择动画（高亮变回常态）
	OnStopWaitForEquipSelection.Broadcast(AbilityDA->FindAbilityInfoFromTag(AbilityTag).AbilityType);
	OnSpellGlobeReassign.Broadcast(AbilityTag);

}
