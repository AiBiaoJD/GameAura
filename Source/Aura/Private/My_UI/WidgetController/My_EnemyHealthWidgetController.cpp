// Copyright ABiao


#include "My_UI/WidgetController/My_EnemyHealthWidgetController.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_EnemyHealthWidgetController::BroadcastInitiaValues()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	//---------更新UI使用广播委托,传给Widget--------------
	OnEnemyHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnEnemyMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
}

void UMy_EnemyHealthWidgetController::BindCallbacksToDependencies()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
		{
			OnEnemyHealthChanged.Broadcast(Date.NewValue);
		});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
		{
			OnEnemyMaxHealthChanged.Broadcast(Date.NewValue);
		});
}

