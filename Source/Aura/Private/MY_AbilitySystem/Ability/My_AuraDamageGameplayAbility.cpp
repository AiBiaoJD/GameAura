// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMy_AuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
	{
	FGameplayEffectSpecHandle DamageSpecHandle =  MakeOutgoingGameplayEffectSpec(DamageEffectClass,1.f);
	for (TTuple<FGameplayTag,FScalableFloat> Pair : DamageTypes)
	{
		DamageSpecHandle.Data->SetSetByCallerMagnitude(Pair.Key,Pair.Value.GetValueAtLevel(GetAbilityLevel()));
	}
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data,UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}
