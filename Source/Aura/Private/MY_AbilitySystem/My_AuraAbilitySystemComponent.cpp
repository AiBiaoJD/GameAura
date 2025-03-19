// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

#include "My_AuraGamePlayTags_Singleton.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"

UMy_AuraAbilitySystemComponent::UMy_AuraAbilitySystemComponent()
{
	SetIsReplicated(true);
}

//在Character_Base的SetAbilityActorInfo()函数中调用
void UMy_AuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//-------一旦Actorinfo设置好,就可以对ASC的委托进行绑定-----------------
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMy_AuraAbilitySystemComponent::ClientEffectApplied);
}

void UMy_AuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
	const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	//使用委托进行TagContainer的广播,这样WidgetController就可以接受到
	//这样My_ASC不知道WidgetController,而WidgetController知道ASC
	EffectAssetTags.Broadcast(TagContainer);
}

/*
 * 在角色初始化时,给能力添加Tag
 */
void UMy_AuraAbilitySystemComponent::AddCharacterAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbility)
{
	for (auto& Ability : StartupAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);

		if (const UMy_AuraGameplayAbilityBase* AuraAbility = Cast<UMy_AuraGameplayAbilityBase>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);
			GiveAbility(AbilitySpec);
		}
	}
}

/*
 * 当PlayerController 按下/放开 按键会激活下面的函数
 * 根据inputTag激活对应的能力
 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UMy_AuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}


