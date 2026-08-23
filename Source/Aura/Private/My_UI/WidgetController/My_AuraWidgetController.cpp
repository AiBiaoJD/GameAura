// Copyright ABiao


#include "My_UI/WidgetController/My_AuraWidgetController.h"

void UMy_AuraWidgetController::SetWidgetControllerParams(const FMY_WidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMy_AuraWidgetController::BroadcastInitiaValues()
{
}

void UMy_AuraWidgetController::BindCallbacksToDependencies()
{
}

void UMy_AuraWidgetController::BroadcastAbilityInfo()
{
	//获取所有能力的信息，并查找Abilityinfo获取特定info，再传入到widget
	if (!GetAuraASC()->bStartupAbilityGiven) return;

	//绑定Fmy_ForEachAbility委托的回调函数
	Fmy_ForEachAbility OnEachAbility;
	OnEachAbility.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		//跳过被动等没有配 My_Abilities AbilityTag 的能力（查不到对应 AbilityInfo，会报 [None] 错误）
		const FGameplayTag AbilityTag = GetAuraASC()->GetAbilityTagFromAbilitySpec(AbilitySpec);
		if (AbilityTag.IsValid())
		{
			check(AbilityDA);
			FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTag);
			info.InputTag = GetAuraASC()->GetInputTagFromAbilitySpec(AbilitySpec);
			OnAbilityInfo.Broadcast(info);
		}
	});

	//广播Fmy_ForEachAbility
	GetAuraASC()->ForEachAbility(OnEachAbility);
}

AMy_Aura_Controller* UMy_AuraWidgetController::GetAuraPC()
{
	if (AuraPlayerController == nullptr)
	{
		AuraPlayerController = Cast<AMy_Aura_Controller>(PlayerController);
	}
	return AuraPlayerController;
}

AMy_AuraPlayerState* UMy_AuraWidgetController::GetAuraPS()
{
	if (AuraPlayerState == nullptr)
	{
		AuraPlayerState = Cast<AMy_AuraPlayerState>(PlayerState);
	}
	return AuraPlayerState;
}

UMy_AuraAbilitySystemComponent* UMy_AuraWidgetController::GetAuraASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent);
	}
	return AuraAbilitySystemComponent;
}

UMy_AuraAttributeSet* UMy_AuraWidgetController::GetAuraAS()
{
	if (AuraAttributeSet == nullptr)
	{
		AuraAttributeSet = Cast<UMy_AuraAttributeSet>(AttributeSet);
	}
	return AuraAttributeSet;
}
