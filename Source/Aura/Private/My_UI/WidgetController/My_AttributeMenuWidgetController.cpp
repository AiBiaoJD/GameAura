// Copyright ABiao


#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "My_Controler/My_AuraPlayerState.h"


void UMy_AttributeMenuWidgetController::BroadcastInitiaValues()
{
	check(AttributeDA);

	for (auto& Pair : GetAuraAS()->M_TagsToAttribute)
	{
		BroadAttributeInfo(Pair.Key, Pair.Value());
	}

	if (GetAuraPS())
	{
		OnPlayerAttributeChanged.Broadcast(GetAuraPS()->GetAttributePoint());
	}
}

void UMy_AttributeMenuWidgetController::BindCallbacksToDependencies()
{
	check(AttributeDA);

	for (auto& Pair : GetAuraAS()->M_TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadAttributeInfo(Pair.Key, Pair.Value());
		});
	}
	if (GetAuraPS())
	{
		GetAuraPS()->OnAttributePointChanged.AddLambda([this](int32 NewAttributePoint)
		{
			OnPlayerAttributeChanged.Broadcast(NewAttributePoint);
		});
	}
}

void UMy_AttributeMenuWidgetController::BroadAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute)
{
	FMy_AuraAttributeInfo Info = AttributeDA->FindAttributeinfoFormTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	OnAttributeInfo.Broadcast(Info);
}

void UMy_AttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	GetAuraASC()->UpgradeAttribute(AttributeTag);
}
