// Copyright ABiao


#include "My_UI/WidgetController/My_OverlayWidgetController.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void UMy_OverlayWidgetController::BroadcastInitiaValues()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	// 触发委托，通知所有绑定的函数
	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

}

//ATTributeSet里面属性发生改变，调用的回调函数
void UMy_OverlayWidgetController::BindCallbacksToDependencies()
{
	const UMy_AuraAttributeSet* AuraAttributeSet = CastChecked<UMy_AuraAttributeSet>(AttributeSet);
	//生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::HealthChanged);

	//最大生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UMy_OverlayWidgetController::MaxHealthChanged);

	//法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute()).AddUObject(this, &UMy_OverlayWidgetController::ManaChanged);

	//最大法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute()).AddUObject(this, &UMy_OverlayWidgetController::MaxManaChanged);

	//绑定ASC的委托
	Cast<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				const FString Msg = FString::Printf(TEXT("GE Tag: %s"), *Tag.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 8, FColor::Blue, Msg);

				//获取数据表标签为Tag的Row
				FMy_UIWidgetRow* Row = GetDataTableRowByTag<FMy_UIWidgetRow>(MessageWidgetDataTable, Tag);

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
