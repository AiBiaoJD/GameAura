// Copyright ABiao


#include "Aura/Public/My_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UMy_AuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	DamageSpecHandle.Data->SetSetByCallerMagnitude(DamageType, Damage.GetValueAtLevel(GetAbilityLevel()));
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data, UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
}

FMy_DamageEffectParams UMy_AuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor,
                                                                                              FVector InRadialDamageOrigin) const
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

	/*
	 * 冲量源点：决定击退 / 死亡冲量朝哪个方向飞
	 *   普通技能 = 施法者位置（从哪打来的就往哪飞）
	 *   径向伤害 = 球心（爆炸把人往外推）
	 * 先定源点，下面算方向时只写一遍 —— 不重复掷骰，也不用每个 GA 各管各的
	 */
	FVector ImpulseOrigin = GetAvatarActorFromActorInfo()->GetActorLocation();

	if (bIsRadialDamage)
	{
		Params.bIsRadialDamage = bIsRadialDamage;
		// 没传球心就退回施法者脚下：ArcaneShards 这种「以自己为中心」的技能直接用
		Params.RadialDamageOrigin = InRadialDamageOrigin.IsNearlyZero()
			                            ? GetAvatarActorFromActorInfo()->GetActorLocation()
			                            : InRadialDamageOrigin;
		Params.RadialDamageInnerRadius = RadialDamageInnerRadius;
		Params.RadialDamageOuterRadius = RadialDamageOuterRadius;

		// 径向伤害的冲量从球心向外推
		ImpulseOrigin = Params.RadialDamageOrigin;
	}

	// 冲量方向 = 从源点指向目标。
	// 只有「施法瞬间就知道打谁」的 GA 才传 TargetActor（敌人 MeleeAttack、玩家 Electrocute）；
	// 投射物类传 nullptr（FireBolt / ProjectileSpell），这里整块跳过 ——
	// 因为发射时还不知道会撞上谁，等命中时由投射物按飞行方向自己算。
	if (IsValid(TargetActor))
	{
		/*
		 * 目标几乎站在球心上时（碎片点位正好落在脚下），这个向量接近零，
		 * Rotation() 会退化成 (1,0,0)，人被往 +X 平推，看着像被莫名踢了一脚。
		 * 这种情况改成纯垂直向上打飞 —— 站在爆心就该原地飞起来。
		 *
		 * 只比较水平距离：点位贴在地面上，而目标是胶囊体中心（离地约 88），
		 * 直接用 3D 距离的话 Z 差就把阈值撑爆了，站在点位上也会判成「不重合」。
		 * 清零 Z 不影响方向 —— 下面 Rotator.Pitch 会被重新赋值，只用到 Yaw。
		 */
		FVector ToTargetDir = TargetActor->GetActorLocation() - ImpulseOrigin;
		ToTargetDir.Z = 0.f;
		const float HorizontalDist = ToTargetDir.Size();

		bool bVerticalLaunch = false;
		if (HorizontalDist < 30.f)
		{
			ToTargetDir = FVector::UpVector;
			bVerticalLaunch = true;
		}

		FRotator Rotator = ToTargetDir.Rotation();
		// 45 度是给普通击退加抛物线弧度；垂直起飞时不改 Pitch，保持 90 度朝正上方
		if (!bVerticalLaunch)
		{
			Rotator.Pitch = 45.f;
		}
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

float UMy_AuraDamageGameplayAbility::GetDamageAtLevel()
{
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	return ScaledDamage;
}
