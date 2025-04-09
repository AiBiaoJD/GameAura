// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "My_AbilityActor/My_ProjectileActor.h"
#include "My_Interraction/My_CombatInterface.h"


void UMy_AuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
void UMy_AuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
    // 确保有有效的 AvatarActor
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor)
    {
        EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
        return;
    }

    // 检查 CombatInterface
    IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(AvatarActor);
    if (!CombatInterface)
    {
        EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
        return;
    }

    // 计算生成位置和旋转
    const FVector SockLoc = CombatInterface->GetWeaponSockLocation();
    FRotator Rotation = (ProjectileTargetLocation - SockLoc).Rotation();
    Rotation.Pitch = 0.f;

    FTransform SpawnTransform;
    SpawnTransform.SetRotation(Rotation.Quaternion());
    SpawnTransform.SetLocation(SockLoc + Rotation.Vector() * 50.f);

    // 检查是否是本地控制的玩家
    APawn* AvatarPawn = Cast<APawn>(AvatarActor);
    bool bIsLocallyControlled = AvatarPawn && AvatarPawn->IsLocallyControlled();

    if (!AvatarActor->HasAuthority())
    {
        // 客户端逻辑
        if (bIsLocallyControlled)
        {
            // 本地玩家生成预测投射物
            if (AMy_ProjectileActor* ClientProjectile = GetWorld()->SpawnActor<AMy_ProjectileActor>(ProjectileClass, SpawnTransform))
            {
                UE_LOG(LogTemp, Warning, TEXT("Client Predicted Projectile"));
                ClientProjectile->bIsPredicted = true;
                ClientProjectile->DamageEffectSpecHandle = FGameplayEffectSpecHandle();
                float Timestamp = GetWorld()->GetTimeSeconds();
                ClientPredictedProjectiles.Add(Timestamp, ClientProjectile);
            }
        }
        // 发送 RPC 到服务器
        ServerSpawnProjectile(SpawnTransform, ProjectileTargetLocation, GetWorld()->GetTimeSeconds());
    }
    else
    {
        // 服务器逻辑
        // SpawnProjectileInternal(SpawnTransform, ProjectileTargetLocation, GetWorld()->GetTimeSeconds());
    }
}

// 服务器RPC实现
void UMy_AuraProjectileSpell::ServerSpawnProjectile_Implementation(const FTransform& SpawnTransform, const FVector& TargetLocation, float ClientTimestamp)
{
	SpawnProjectileInternal(SpawnTransform, TargetLocation, ClientTimestamp);
}

void UMy_AuraProjectileSpell::SpawnProjectileInternal(const FTransform& SpawnTransform, const FVector& TargetLocation, float ClientTimestamp)
{
	// 确保 ProjectileClass 有效
	if (!ProjectileClass) return;

	// 获取当前 Ability 的 ActivationInfo
	const FGameplayAbilityActivationInfo& ActivationInfo = GetCurrentActivationInfo();

	// 检查是否在服务器上运行（使用 UGameplayAbility 的 HasAuthority）
	const bool bIsServer = HasAuthority(&ActivationInfo); // 传入 ActivationInfo

	// 仅服务器生成权威投射物（客户端预测投射物已在 SpawnProjectile 生成）
	AMy_ProjectileActor* Projectile = GetWorld()->SpawnActorDeferred<AMy_ProjectileActor>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!Projectile) return;

	// 仅服务器设置伤害（客户端预测投射物不处理伤害）
	if (bIsServer)
	{
		const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		if (SourceASC && DamageEffectClass)
		{
			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(Projectile);

			FHitResult HitResult;
			HitResult.Location = TargetLocation;
			EffectContextHandle.AddHitResult(HitResult);

			const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
				DamageEffectClass,
				GetAbilityLevel(),
				EffectContextHandle
			);

			// 设置伤害值
			for (const auto& Pair : DamageTypes)
			{
				const float DamageValue = Pair.Value.GetValueAtLevel(GetAbilityLevel());
				SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, DamageValue);
			}

			Projectile->DamageEffectSpecHandle = SpecHandle;
		}

	}

	Projectile->FinishSpawning(SpawnTransform);
}
