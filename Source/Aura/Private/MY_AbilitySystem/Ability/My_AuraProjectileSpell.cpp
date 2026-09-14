// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "My_AbilityActor/My_ProjectileActor.h"
#include "My_Interraction/My_CombatInterface.h"


void UMy_AuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UMy_AuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride)
{
	// ProjectileActor需要在服务器生成,replicate到客户端
	// 此时Ability是预测的，不会受这个影响，会生成预测火球
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	/*
	 * 这里不直接获取AuraCharter,而是使用Interface结构
	 * 是为了让Ability不绑定Aura角色,可以检查任意的角色/敌人
	 */
	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SockLoc = IMy_CombatInterface::Execute_GetWeaponSockLocation(GetAvatarActorFromActorInfo(), SocketTag);
		FRotator Rotation = (ProjectileTargetLocation - SockLoc).Rotation();
		if (bOverridePitch)
		{
			Rotation.Pitch = PitchOverride;
		}

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SockLoc);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		// 因为要在ProjectileActor中添加Effect,所以使用这种方式创建,方便在生成Actor时Effect已经添加
		AMy_ProjectileActor* Projectile = GetWorld()->SpawnActorDeferred<AMy_ProjectileActor>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		for (auto& Pair : DamageTypes)
		{
			float DamageValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
			SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, DamageValue);
		}
		Projectile->DamageEffectSpecHandle = SpecHandle;
		Projectile->FinishSpawning(SpawnTransform);
	}
}

FString UMy_AuraProjectileSpell::GetDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FMy_AuraGameplayTags::GetInstance().My_DamageType_Fire].GetValueAtLevel(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT("<Title>焰矢</>\n\n<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧</>\n\n<Small>当前等级 </><Level>%d</>"), Damage, Level);
	}
	else
	{
		return FString::Printf(TEXT("<Title>焰矢</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧</>\n\n<Small>当前等级 </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
	}
}

FString UMy_AuraProjectileSpell::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = DamageTypes[FMy_AuraGameplayTags::GetInstance().My_DamageType_Fire].GetValueAtLevel(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧</>\n\n<Small>升级后等级 </><Level>%d</>"), FMath::Min(Level, NumProjectiles), Damage, Level);
}
