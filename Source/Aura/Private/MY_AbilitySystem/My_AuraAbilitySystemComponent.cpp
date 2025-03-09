// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

UMy_AuraAbilitySystemComponent::UMy_AuraAbilitySystemComponent()
{
	SetIsReplicated(true);
}

//在Character_Base的SetAbilityActorInfo()函数中调用
void UMy_AuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//-------一旦Actorinfo设置好,就可以对ASC的委托进行绑定-----------------
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMy_AuraAbilitySystemComponent::EffectApplied);
}

void UMy_AuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	//使用委托进行TagContainer的广播,这样WidgetController就可以接受到
	//这样My_ASC不知道WidgetController,而WidgetController知道ASC
	EffectAssetTags.Broadcast(TagContainer);

	
}
