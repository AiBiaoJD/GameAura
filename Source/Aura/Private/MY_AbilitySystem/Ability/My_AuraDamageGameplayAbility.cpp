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
	Params.KnockbackChance = KnockbackChance;
	Params.KnockbackMagnitude = KnockbackMagnitude;
	// 这里是为了给蓝图中敌人MeleeAttack这些GA使用的
	// 因为这些GA没有context需要创建从这里获取context来设置他们的击退功能这些
	// 而这个时候不同于Projectile这些他们在碰撞的时候处理deathImpulse和KnockBack这些
	// 所以给一个默认值给他们这些GA
	if (IsValid(TargetActor))
	{
		FRotator Rotator = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		Rotator.Pitch = 45.f;
		const FVector ToTarget = Rotator.Vector();
		// 死亡冲量不需要过几率：死了就该被打飞（Magnitude 配 0 时才不动）
		Params.DeathImpulse = ToTarget * DeathImpulseMagnitude;
		// ★ 击退要过几率：Chance = 0 时永远不击退（修复：原来是无条件赋值，Chance 完全不起作用）
		//   掷骰放在【参数生成阶段】而不是 HandleIncomingDamage 里 ——
		//   因为 Debuff 的 DOT 每 tick 也会走 HandleIncomingDamage，
		//   放那里会变成"被灼烧时每 0.5 秒重新掷一次骰"，烧着烧着人就被推走。
		//   一次攻击只掷一次骰。
		if (FMath::RandRange(1, 100) < KnockbackChance)
		{
			Params.Knockback = ToTarget * KnockbackMagnitude;
		}
	}
	return Params;
}
