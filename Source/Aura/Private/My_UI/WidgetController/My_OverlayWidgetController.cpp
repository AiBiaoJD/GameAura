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

//ATTributeSet里面属性发生改变，调用的回调函数
void UMy_OverlayWidgetController::BindCallbacksToDependencies()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);
	//生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::HealthChanged);


	//最大生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::MaxHealthChanged);
}

void UMy_OverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Date) const
{
	//NewNewValue是FOnAttributeChangeData结构的一个变量
	//为什么这个变量是Health，因为GetGameplayAttributeValueChangeDelegate委托绑定了Health
	OnHealthChanged.Broadcast(Date.NewValue);
}

void UMy_OverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Date) const
{
	OnMaxHealthChanged.Broadcast(Date.NewValue);

}
