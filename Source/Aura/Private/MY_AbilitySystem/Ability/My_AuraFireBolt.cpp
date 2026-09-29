// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraFireBolt.h"

#include "Kismet/KismetSystemLibrary.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "My_Interraction/My_CombatInterface.h"


FString UMy_AuraFireBolt::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		// 1 级专属文案：发射数写死为 1，并额外提示升级后的成长
		return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>1</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>\n\n<Small>升级后可同时发射更多焰矢。</>"), ManaCost, Cooldown, ScaledDamage);
	}

	// 高等级：发射数随等级增加，上限为 NumProjectiles
	const int32 NumProj = FMath::Min(Level, NumProjectiles);
	return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>"), Level, ManaCost, Cooldown, NumProj, ScaledDamage);
}

FString UMy_AuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const int32 NumProj = FMath::Min(Level, NumProjectiles);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>"), Level, ManaCost, Cooldown, NumProj, ScaledDamage);
}

void UMy_AuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	// ===== 优先判断服务器权限，客户端直接return =====
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(GetAvatarActorFromActorInfo());
	if (!CombatInterface) return;

	const FVector SockLoc = IMy_CombatInterface::Execute_GetWeaponSockLocation(GetAvatarActorFromActorInfo(), SocketTag);

	// 原始瞄准旋转：从发射点指向目标点的方向
	FRotator BaseRotation = (ProjectileTargetLocation - SockLoc).Rotation();

	// ========== 【先计算距离】 ==========
	float Dist = 0.f;
	if (HomingTarget && HomingTarget->Implements<UMy_CombatInterface>())
	{
		// 敌人目标：使用敌人Actor真实位置计算距离
		Dist = FVector::Dist(SockLoc, HomingTarget->GetActorLocation());
	}
	else
	{
		// 地面点目标：使用传入的目标位置
		Dist = FVector::Dist(SockLoc, ProjectileTargetLocation);
	}
	// ✅ 只有【远距离】并且入参bOverridePitch为true，才抬高Pitch；近距离直接使用原始瞄准角度，不抬高
	if (bOverridePitch && Dist >= HomingMinDistance)
	{
		BaseRotation.Pitch = PitchOverride;
	}

	// ========== 远近全部启用扇形扩散 ==========
	NumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotations = UMy_AuraAbilitySystemLibrary::GetEvenSpreadRotators(BaseRotation.Vector(), FVector::UpVector, SpawnSpread, NumProjectiles);

	for (FRotator& Rot : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SockLoc);
		SpawnTransform.SetRotation(Rot.Quaternion());

		// 因为要在ProjectileActor中添加Effect,所以使用SpawnActorDeferred延迟生成
		AMy_ProjectileActor* Projectile = GetWorld()->SpawnActorDeferred<AMy_ProjectileActor>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(nullptr);

		// ========== 远近全部开启追踪 ==========
		// ★ 这里【不再直接设 ProjectileMovement】——原因见 My_ProjectileActor.h 里那段说明：
		//   UProjectileMovementComponent 一个属性都不复制，而本函数开头就
		//   if (!HasAuthority()) return; → 客户端根本收不到追踪设置 → 那边是直线飞。
		//
		//   现在改成：只把"追踪所需要的信息"交给投射物（这些是复制属性），
		//             由 AMy_ProjectileActor::ApplyHomingSetup() 在两端各自组装。
		const bool bTrackActor = HomingTarget && HomingTarget->Implements<UMy_CombatInterface>();

		Projectile->HomingTargetActor    = bTrackActor ? HomingTarget : nullptr;
		Projectile->HomingTargetLocation = bTrackActor ? FVector::ZeroVector : ProjectileTargetLocation;
		Projectile->bHomingEnabled       = bLaunchHomingProjectile;

		// ★ 加速度是随机的（FRandRange）：必须在【服务器这里骰一次】，然后把结果复制过去。
		//   如果让客户端自己再骰一次，两端得到的值不同 → 轨迹还是对不上。
		Projectile->HomingAcceleration   = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);

		Projectile->FinishSpawning(SpawnTransform);
	}
}
