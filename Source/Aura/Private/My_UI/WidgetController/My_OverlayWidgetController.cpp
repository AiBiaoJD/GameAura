// Copyright ABiao


#include "My_UI/WidgetController/My_OverlayWidgetController.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_OverlayWidgetController::BroadcastInitiaValues()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	// 触发委托，通知所有绑定的函数
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

}
