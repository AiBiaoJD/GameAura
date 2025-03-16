// Copyright ABiao


#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_AttributeMenuWidgetController::BroadcastInitiaValues()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	check(AttributeDA);

	for (auto& Pair : AS->M_TagsToAttribute)
	{
		BroadAttributeInfo(Pair.Key, Pair.Value());
	}

}

void UMy_AttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);
	check(AttributeDA);

	for (auto& Pair : AS->M_TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadAttributeInfo(Pair.Key, Pair.Value());
			});
	}
}

void UMy_AttributeMenuWidgetController::BroadAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute)
{
	FMy_AuraAttributeInfo Info = AttributeDA->FindAttributeinfoFormTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);;
	OnAttributeInfo.Broadcast(Info);
}
