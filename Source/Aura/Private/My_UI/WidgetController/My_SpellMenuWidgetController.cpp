// Copyright ABiao


#include "My_UI/WidgetController/My_SpellMenuWidgetController.h"

void UMy_SpellMenuWidgetController::BroadcastInitiaValues()
{
	BroadcastAbilityInfo();
	OnPlayerSpellPointChanged.Broadcast(GetAuraPS()->GetSpellPoint());
}

void UMy_SpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->OnAbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag)
	{
		if (AbilityDA)
		{
			FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTags);
			info.StatusTag = StatusTag;
			OnAbilityInfo.Broadcast(info);
		}
	});
	if (GetAuraPS())
	{
		GetAuraPS()->OnSpellPointChanged.AddLambda([this](int32 NewSpellPoint)
		{
			OnPlayerSpellPointChanged.Broadcast(NewSpellPoint);
		});
	}
}
