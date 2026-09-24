// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMy_AuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	DamageSpecHandle.Data->SetSetByCallerMagnitude(DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FMy_DamageEffectParams UMy_AuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const 
{
	FMy_DamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	return Params;
}
