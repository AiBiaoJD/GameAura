// Copyright ABiao


#include "My_UI/WidgetController/My_OverlayWidgetController.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_OverlayWidgetController::BroadcastInitiaValues()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	//---------更新UI使用广播委托,传给Widget--------------
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

//ATTributeSet里面属性发生改变，调用的回调函数
void UMy_OverlayWidgetController::BindCallbacksToDependencies()
{
	//---------属性改变使用ASC的绑定委托,传给ASC--------------
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);
	//生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::HealthChanged);

	//最大生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::MaxHealthChanged);

	//法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UMy_OverlayWidgetController::ManaChanged);

	//最大法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UMy_OverlayWidgetController::MaxManaChanged);


	//---------Effect Applied使用ASC的绑定委托,传给My_ASC--------------
	Cast<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				//"Message.HealthPotion".MatchesTag("Message") will return True, "
				//Message".MatchesTag("Message.HealthPotion") will return False
				//************ Message Tag ****************
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("My_Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					//获取数据表标签为Tag的Row
					const FMy_UIWidgetRow* Row = GetDataTableRowByTag<FMy_UIWidgetRow>(MessageWidgetDataTable, Tag);


					//---------广播DataTable的row使用广播委托,传给Widget--------------
					OnMessageWidgetRow.Broadcast(*Row);
				}


			}
		}
	);

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

void UMy_OverlayWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UMy_OverlayWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);

}
