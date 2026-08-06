
// Copyright ABiao


#include "My_UI/WidgetController/My_OverlayWidgetController.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "MY_AbilitySystem/Data/My_AbilityInfo.h"
#include "MY_AbilitySystem/Data/My_LevelUpInfo.h"
#include "My_Controler/My_AuraPlayerState.h"

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

	//XP和Level改变时添加委托绑定函数
	AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
	if (MyPS)
	{
		MyPS->OnXPChanged.AddUObject(this, &UMy_OverlayWidgetController::OnXPChangedFunc);
		MyPS->OnLevelChanged.AddLambda([this](int32 NewLevel)
		{
			OnPlayerLevelChanged.Broadcast(NewLevel);
		});
	}
	
	if (UMy_AuraAbilitySystemComponent* AuraASC = Cast<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		//---------Effect Applied使用ASC的绑定委托,传给My_ASC--------------
		AuraASC->EffectAssetTags.AddLambda(
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
		if (AuraASC->bStartupAbilityGiven)
		{
			OnInitializeStartupAbilities(AuraASC);
		}
		else
		{
			/*还没进行广播绑定回调函数*/
			AuraASC->OnAbilityGiven.AddUObject(this, &UMy_OverlayWidgetController::OnInitializeStartupAbilities);
		}
	}
}

void UMy_OverlayWidgetController::OnXPChangedFunc(int32 NewXP)
{
	AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
	checkf(MyPS && MyPS->LevelUpInfo, TEXT("LevelUpInfo not set"));

	const int32 Level = MyPS->LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = MyPS->LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpReq = MyPS->LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PrevLevelUpReq = MyPS->LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;
		
		const float XPBarPercent = static_cast<float>(NewXP-PrevLevelUpReq) / static_cast<float>(LevelUpReq - PrevLevelUpReq);
		OnXPPercentChanged.Broadcast(XPBarPercent);
	}
}

void UMy_OverlayWidgetController::OnInitializeStartupAbilities(UMy_AuraAbilitySystemComponent* AuraASC)
{
	//获取所有能力的信息，并查找Abilityinfo获取特定info，再传入到widget
	if (!AuraASC->bStartupAbilityGiven) return;

	//绑定Fmy_ForEachAbility委托的回调函数
	Fmy_ForEachAbility OnEachAbility;
	OnEachAbility.BindLambda([this,AuraASC](const FGameplayAbilitySpec& AbilitySpec)
	{
		FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AuraASC->GetAbilityTagFromAbilitySpec(AbilitySpec));
		info.InputTag = AuraASC->GetInputTagFromAbilitySpec(AbilitySpec);
		OnAbilityInfo.Broadcast(info);
	});

	//广播Fmy_ForEachAbility
	AuraASC->ForEachAbility(OnEachAbility);
}
