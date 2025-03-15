// Copyright ABiao


#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"

#include "My_AuraGamePlayTags_Singleton.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_AttributeMenuWidgetController::BroadcastInitiaValues()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	check(AttributeDA);

	//从DataAsset中查找Tag,DataAsset是继承自UMy_Attributeinfo
	FMy_AuraAttributeInfo info = AttributeDA->FindAttributeinfoFormTag(FMy_AuraGameplayTags::GetInstance().My_Attribute_Primary_Strength);

	info.AttributeValue = AS->GetStrength();

	OnAttributeInfo.Broadcast(info);
}

void UMy_AttributeMenuWidgetController::BindCallbacksToDependencies()
{
}
