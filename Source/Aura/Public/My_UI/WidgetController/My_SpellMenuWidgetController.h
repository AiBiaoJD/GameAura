// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_SpellMenuWidgetController.generated.h"

// 选中技能球后的回调：通知 UI 两个按钮（花点/装备）是否可用
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMy_SpellGlobeSelectSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled);

// 记录"当前选中的技能 + 它最新的状态"
// 为什么必须缓存 StatusTag：
//   - ASC 的 StatusTag 与 PS 的 SpellPoint 是两条独立的网络复制通道，到达客户端的时间/顺序不确定
//   - 状态无法随时现读：Locked（未授予）时根本没有 Spec 可读，已授予的 Spec 复制也可能有延迟
//   - 所以把回调送来的权威 StatusTag 存下来，供两个回调重算按钮时使用（详见 .cpp 的 BindCallbacksToDependencies 注释）
struct FMy_SelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag StatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UMy_SpellMenuWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "My_GAS|SpellPoints")
	FMy_OnPlayerStateChangedSignature OnPlayerSpellPointChanged;

	UPROPERTY(BlueprintAssignable)
	FMy_SpellGlobeSelectSignature OnSpellGlobeSelect;

	// 点击技能球时调用：算出状态并广播按钮可用状态
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendPointsButtonPressed();

	virtual void BroadcastInitiaValues() override;
	virtual void BindCallbacksToDependencies() override;

	
private:
	// 根据 技能状态 + 法术点数 决定 花点/装备 按钮是否可用
	static void My_ShouldEnableButton(FGameplayTag AbilityStatus, const int32 SpellPoint, bool& bSpendPointsButtonEnabled, bool& bEquipButtonEnabled);

	// 缓存当前选中技能的状态（ASC 复制不可靠，见 struct 注释）
	FMy_SelectedAbility SelectedAbility = {FMy_AuraGameplayTags::GetInstance().My_Abilities_None, FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Locked};
};
