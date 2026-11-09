// Copyright ABiao


#include "My_AbilitySystem/Ability/My_AuraFireBlast.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Pawn.h"
#include "My_AbilityActor/My_FireBall.h"
#include "My_AbilitySystem/My_AuraAbilitySystemLibrary.h"

int32 UMy_AuraFireBlast::GetNumFireBallsAtLevel(int32 Level) const
{
	const int32 CurveCount = FMath::RoundToInt(NumFireBalls.GetValueAtLevel(Level));

	// 配了曲线就完全按曲线走，没配曲线才叠 FireBallsPerLevel
	const int32 RawCount = NumFireBalls.IsStatic()
		                        ? CurveCount + FireBallsPerLevel * (Level - 1)
		                        : CurveCount;

	/*
	 * 至少一颗：0 颗会让 GetEvenSpreadRotators 返回空数组，技能变成空放。
	 * 至多 MaxNumFireBalls：球心间距 = 2 × ExplodeDistance × sin(180°/N)，
	 * 球太多球心过密，同一个敌人被一圈球反复覆盖，爆炸伤害会失控。
	 */
	return FMath::Clamp(RawCount, 1, FMath::Max(1, MaxNumFireBalls));
}

FString UMy_AuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ExplosionBase = ExplosionDamage.GetValueAtLevel(Level);
	// 与 SpawnFireBalls 的回落逻辑保持一致，描述里显示的就是实际生效的数值
	const int32 ScaledExplosion = ExplosionBase > 0.f ? FMath::RoundToInt(ExplosionBase) : ScaledDamage;
	const int32 NumBalls = GetNumFireBallsAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>火焰爆裂</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，回到身边时爆炸造成 </><Damage>%d</><Default> 点径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumBalls, ScaledDamage, ScaledExplosion, DebuffChance);
}

FString UMy_AuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ExplosionBase = ExplosionDamage.GetValueAtLevel(Level);
	const int32 ScaledExplosion = ExplosionBase > 0.f ? FMath::RoundToInt(ExplosionBase) : ScaledDamage;
	const int32 NumBalls = GetNumFireBallsAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，回到身边时爆炸造成 </><Damage>%d</><Default> 点径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumBalls, ScaledDamage, ScaledExplosion, DebuffChance);
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
	 * ExplosionDamageParams —— 回到身边时的径向爆炸
	 *
	 * ★ 径向字段必须手动补，不能靠 MakeDamageEffectParamsFromClassDefaults 的第二个参数。
	 *   那个分支的入口条件是 GA 类的 bIsRadialDamage 为真，而 FireBlast 的火球撞人是
	 *   普通单体伤害，GA 上不能勾（勾了撞人也变成径向）。所以球心/内外半径全靠下面四行。
	 */
	const FMy_DamageEffectParams DamageParams = MakeDamageEffectParamsFromClassDefaults();

	FMy_DamageEffectParams ExplosionParams = MakeDamageEffectParamsFromClassDefaults();
	ExplosionParams.bIsRadialDamage = true;
	ExplosionParams.RadialDamageInnerRadius = ExplosionInnerRadius;
	ExplosionParams.RadialDamageOuterRadius = ExplosionOuterRadius;
	/*
	 * 初始球心 = 施法瞬间的位置，只作兜底。
	 * 真正爆炸时 MakeExplosionParamsForTarget 会用「火球自己的位置」覆盖它 ——
	 * 火球飞出去再飞回来要一两秒，玩家和火球都可能已经不在原地了。
	 */
	ExplosionParams.RadialDamageOrigin = Location;

	/*
	 * 爆炸伤害单独一条曲线，和火球撞人的 Damage 分开配。
	 * ExplosionDamage 没配曲线时 GetValueAtLevel 返回 0，这时回落到基类的 Damage ——
	 * 否则爆炸会变成「有特效、有击退，但一点血都不掉」。
	 *
	 * 这里配的是「单颗球的伤害」：球心分散在施法者周围一圈，各炸自己那一小片，
	 * 一个敌人通常被 2~4 颗球命中，最终伤害 = 单颗伤害 × 命中球数（含距离衰减）。
	 */
	const float ExplosionBase = ExplosionDamage.GetValueAtLevel(GetAbilityLevel());
	if (ExplosionBase > 0.f)
	{
		ExplosionParams.BaseDamage = ExplosionBase;
	}

	const int32 BallCount = GetNumFireBallsAtLevel(GetAbilityLevel());

	// 传 360 必须走闭环，否则首尾两颗球方向相同
	const TArray<FRotator> Rotators = UMy_AuraAbilitySystemLibrary::GetEvenSpreadRotators(
		Forward, FVector::UpVector, 360.f, BallCount, true);

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
