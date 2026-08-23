// Copyright ABiao


#include "My_UI/WidgetController/My_OverlayWidgetController.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "MY_AbilitySystem/Data/My_LevelUpInfo.h"
#include "My_Controler/My_AuraPlayerState.h"

void UMy_OverlayWidgetController::BroadcastInitiaValues()
{
	//---------更新UI使用广播委托,传给Widget--------------
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
}

//ATTributeSet里面属性发生改变，调用的回调函数
void UMy_OverlayWidgetController::BindCallbacksToDependencies()
{
	//---------属性改变使用ASC的绑定委托,传给ASC--------------
	//生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
	{
		OnHealthChanged.Broadcast(Date.NewValue);
	});

	//最大生命值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Date)
	{
		OnMaxHealthChanged.Broadcast(Date.NewValue);
	});

	//法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnManaChanged.Broadcast(Data.NewValue);
	});

	//最大法力值改变函数添加到委托
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		OnMaxManaChanged.Broadcast(Data.NewValue);
	});

	//XP和Level改变时添加委托绑定函数

	if (GetAuraPS())
	{
		GetAuraPS()->OnXPChanged.AddUObject(this, &UMy_OverlayWidgetController::OnXPChangedFunc);
		GetAuraPS()->OnLevelChanged.AddLambda([this](int32 NewLevel)
		{
			OnPlayerLevelChanged.Broadcast(NewLevel);
		});
	}

	if (GetAuraASC())
	{
		//---------Effect Applied使用ASC的绑定委托,传给My_ASC--------------
		GetAuraASC()->EffectAssetTags.AddLambda(
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

		//---------玩家获得StartupAbility时，进行的委托绑定--------------
		//处理OnAbilityGiven这个委托先广播brocast再绑定回调函数的问题
		//因为GiveAbility()和HUD创建WidgetController的时间是不确定的
		if (GetAuraASC()->bStartupAbilityGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			/*还没进行广播绑定回调函数*/
			GetAuraASC()->OnAbilityGiven.AddUObject(this, &UMy_OverlayWidgetController::BroadcastAbilityInfo);
		}
	}
}

void UMy_OverlayWidgetController::OnXPChangedFunc(int32 NewXP)
{
	checkf(GetAuraPS() && GetAuraPS()->LevelUpInfo, TEXT("LevelUpInfo not set"));

	const int32 Level = GetAuraPS()->LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = GetAuraPS()->LevelUpInfo->LevelUpInformation.Num();

	if (Level == MaxLevel)
	{
		OnXPPercentChanged.Broadcast(1.0f);
	}
	else
	{
		const int32 LevelUpReq = GetAuraPS()->LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PrevLevelUpReq = GetAuraPS()->LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;
		const float XPBarPercent = static_cast<float>(NewXP - PrevLevelUpReq) / static_cast<float>(LevelUpReq - PrevLevelUpReq);
		OnXPPercentChanged.Broadcast(XPBarPercent);
	}
}
