// Copyright ABiao


#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_AttributeMenuWidgetController::BroadcastInitiaValues()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	check(AttributeDA);

	for (auto& Pair : AS->M_TagsToAttribute)
	{
		FMy_AuraAttributeInfo Info = AttributeDA->FindAttributeinfoFormTag(Pair.Key);
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		OnAttributeInfo.Broadcast(Info);
	}

}

void UMy_AttributeMenuWidgetController::BindCallbacksToDependencies()
{
}
