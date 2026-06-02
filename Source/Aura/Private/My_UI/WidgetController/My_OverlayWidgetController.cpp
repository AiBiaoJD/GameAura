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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
		{
			OnHealthChanged.Broadcast(Date.NewValue);
		});

	//最大生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
		{
			OnMaxHealthChanged.Broadcast(Date.NewValue);

		});

	//法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		});

	//最大法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		});

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



