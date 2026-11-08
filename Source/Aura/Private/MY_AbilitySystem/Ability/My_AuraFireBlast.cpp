// Copyright ABiao


#include "My_AbilitySystem/Ability/My_AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "My_AbilityActor/My_FireBall.h"
#include "My_AbilitySystem/My_AuraAbilitySystemLibrary.h"

FString UMy_AuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>火焰爆裂</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，贴近自身时爆炸造成径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage, DebuffChance);
}

FString UMy_AuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，贴近自身时爆炸造成径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage, DebuffChance);
}

TArray<AMy_FireBall*> UMy_AuraFireBlast::SpawnFireBalls()
{
	TArray<AMy_FireBall*> FireBalls;

	// 火球是复制 Actor，只在服务器生成，客户端靠复制看到。
	// GA 在客户端也会预测执行，不拦的话客户端会多出一批本地幽灵火球
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return FireBalls;

	// 蓝图里没配火球类就直接退出，否则 SpawnActorDeferred 返回空指针，下一行就崩
	if (!FireBallClass) return FireBalls;

	const AActor* Avatar = GetAvatarActorFromActorInfo();
	const FVector Forward = Avatar->GetActorForwardVector();
	const FVector Location  = Avatar->GetActorLocation();

	/*
	 * 两类伤害参数各算一次，循环里只做拷贝：
	 * DamageEffectParams    —— 火球撞到敌人时的直接伤害
	 * ExplosionDamageParams —— 回到身边时的径向爆炸，球心就是施法者位置
	 * 两者只有径向字段不同，所以都从同一份默认值改出来
	 */
	const FMy_DamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults();

	FMy_DamageEffectParams ExplosionParams = MakeDamageEffectParamsFromClassDefaults(nullptr, Location);
	ExplosionParams.bIsRadialDamage = true;
	ExplosionParams.RadialDamageInnerRadius = ExplosionInnerRadius;
	ExplosionParams.RadialDamageOuterRadius = ExplosionOuterRadius;
	ExplosionParams.RadialDamageOrigin = Location;

	// 传 360 必须走闭环，否则首尾两颗球方向相同
	const TArray<FRotator> Rotators = UMy_AuraAbilitySystemLibrary::GetEvenSpreadRotators(
		Forward, FVector::UpVector, 360.f, NumFireBalls, true);

	FireBalls.Reserve(Rotators.Num());

	for (const FRotator& Rotator : Rotators)
	{
		const FTransform SpawnTransform(Rotator, Location);

		// 延迟生成：先填参数再 FinishSpawning，保证 BeginPlay 时数据已就绪
		AMy_FireBall* FireBall = GetWorld()->SpawnActorDeferred<AMy_FireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (!IsValid(FireBall)) continue;

		FireBall->DamageEffectParams = DamageParams;
		FireBall->ExplosionDamageParams = ExplosionParams;

		// 回程目标与最远点都在这里定，蓝图只负责拿去做 VLerp
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->ApexLocation = Location + Rotator.Vector() * FireBallDistance
		                       + FVector(0.f, 0.f, FireBallHeight);

		FireBall->FinishSpawning(SpawnTransform);
		FireBalls.Add(FireBall);
	}

	return FireBalls;
}
