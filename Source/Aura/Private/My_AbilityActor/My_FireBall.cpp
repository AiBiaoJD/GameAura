// Copyright ABiao


#include "My_AbilityActor/My_FireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "My_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

void AMy_FireBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMy_FireBall, ReturnToActor);
	DOREPLIFETIME(AMy_FireBall, ApexLocation);
}

void AMy_FireBall::BeginPlay()
{
	Super::BeginPlay();

	/*
	 * 服务器：ApexLocation 在 SpawnFireBalls 里已经算好，直接开播。
	 * 客户端：此刻复制数据还没到，ApexLocation 仍是 (0,0,0)，
	 *         硬开会朝世界原点飞 —— 交给 OnRep_ApexLocation 触发。
	 */
	if (HasAuthority())
	{
		StartOutgoingTimeline();
	}
}

void AMy_FireBall::OnRep_ApexLocation()
{
	StartOutgoingTimeline();
}

FMy_DamageEffectParams AMy_FireBall::MakeExplosionParamsForTarget(AActor* TargetActor) const
{
	FMy_DamageEffectParams Params = ExplosionDamageParams;

	/*
	 * 球心 = 火球自己的位置。
	 *
	 * 12 颗球爆炸时分布在施法者周围一圈，各自炸自己那一小片 ——
	 * 正东的敌人只被东边那颗球炸到，不会被西边那颗波及。
	 *
	 * ★ 不能统一用施法者位置：那样 12 个球心完全重合、范围也完全重合，
	 *   每个敌人都吃满 NumFireBalls 次伤害，失去「每个方向各管一片」的设计。
	 *
	 * 用火球位置还顺带解决了玩家移动的问题 —— 火球本来就是追着玩家飞的。
	 */
	Params.RadialDamageOrigin = GetActorLocation();

	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!IsValid(TargetActor)) return Params;

	/*
	 * 冲量方向 = 球心 → 目标（水平化）。
	 * 球心就是施法者位置，所以围在角色周围的怪会各自朝自己那一侧飞出去。
	 */
	FVector ToTarget = TargetActor->GetActorLocation() - Params.RadialDamageOrigin;
	ToTarget.Z = 0.f;

	// 站在爆心正下方时水平方向会退化成随机值，改成纯垂直向上打飞
	const bool bAtCenter = ToTarget.Size() < 30.f;

	FRotator Rotator = bAtCenter ? FRotator(90.f, 0.f, 0.f) : ToTarget.Rotation();
	if (!bAtCenter)
	{
		// 45 度抬升，给一个抛物线弧度；垂直起飞时保持 90 度不变
		Rotator.Pitch = 45.f;
	}

	Params.DeathImpulse = Rotator.Vector() * Params.DeathImpulseMagnitude;

	// 击退和普通攻击一样过几率，每个目标各掷一次
	if (FMath::RandRange(1, 100) < Params.KnockbackChance)
	{
		Params.Knockback = Rotator.Vector() * Params.KnockbackMagnitude;
	}

	return Params;
}

void AMy_FireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority()) return;
	// 碰到施法者
	if (DamageEffectParams.SourceASC->GetAvatarActor() == OtherActor) return;
	// 碰到友军？
	if (!UMy_AuraAbilitySystemLibrary::IsNotFriend(DamageEffectParams.SourceASC->GetAvatarActor(), OtherActor)) return;

	// 碰到敌人！
	// 激活Effect,只能在服务器修改Attribute,Replicate Attribute到客户端
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.DeathImpulse = DeathImpulse;
		
		DamageEffectParams.TargetASC = TargetASC;

		UMy_AuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}
}
