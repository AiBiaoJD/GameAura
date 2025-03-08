// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

//在Character_Base的SetAbilityActorInfo()函数中调用
void UMy_AuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//一旦Actorinfo设置好,就可以对ASC的委托进行绑定
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMy_AuraAbilitySystemComponent::EffectApplied);
}

void UMy_AuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	GEngine->AddOnScreenDebugMessage(1, 8.0f, FColor::Blue, FString("Effect Applied"));
}
