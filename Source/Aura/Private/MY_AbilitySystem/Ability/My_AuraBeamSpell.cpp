// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraBeamSpell.h"

#include "My_AuraGamePlayTags_Singleton.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "My_Interraction/My_CombatInterface.h"

void UMy_AuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UMy_AuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo && CurrentActorInfo->PlayerController.IsValid())
	{
		OwnerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UMy_AuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	if (!OwnerCharacter) return;

	if (OwnerCharacter->Implements<UMy_CombatInterface>())
	{
		const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();
		const FVector SocketLocation = IMy_CombatInterface::Execute_GetWeaponSockLocation(OwnerCharacter, GameplayTags.My_CombatSocket_Weapon);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(OwnerCharacter);

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			SocketLocation,
			BeamTargetLocation,
			10.f,
			TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::None,
			HitResult,
			true
		);

		if (HitResult.bBlockingHit)
		{
			//命中目标逻辑
			MouseHitLocation = HitResult.ImpactPoint;
			MouseHitActor = HitResult.GetActor();
		}
	}

	if (IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(MouseHitActor))
	{
		FMy_DeathSignature& OnDeath = CombatInterface->GetOnDeath();          // ① 拿引用
		if (!OnDeath.IsAlreadyBound(this, &UMy_AuraBeamSpell::PrimaryTargetDie))   // ② 查重
		{
			OnDeath.AddDynamic(this, &UMy_AuraBeamSpell::PrimaryTargetDie);        // ③ 绑
		}
	}
}

void UMy_AuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetAvatarActorFromActorInfo());
	IgnoreActors.Add(MouseHitActor);
	TArray<AActor*> OverlappingActors;
	
	UMy_AuraAbilitySystemLibrary::GetLivePlayersWithRadius(GetAvatarActorFromActorInfo(), OverlappingActors,IgnoreActors,850.f,MouseHitActor->GetActorLocation());

	/*
	 * 电击只连锁敌人。
	 * GetLivePlayersWithRadius 是按 IMy_CombatInterface 收人的，附近的其他玩家同样会被收进来
	 * （它只排除了施法者和主目标）。连锁到队友有两个后果：
	 *   1. 给他挂 My_Enemy.Status.ShockLoop + GameplayCue.My_ShockLoop，对玩家没有意义
	 *   2. 对他打 Lightning 伤害 —— DamageToDebuff 里 Lightning 对应的 Debuff 是 Stun，
	 *      等于顺手把队友控住，多人下就是实打实的友伤
	 * 这里用现成的 IsNotFriend（靠 Actor Tag Player/Enemy 判阵营，My_ProjectileActor 也在用）
	 * 把同阵营的过滤掉。
	 */
	if (AActor* SourceAvatar = GetAvatarActorFromActorInfo())
	{
		OverlappingActors.RemoveAll([SourceAvatar](AActor* Candidate)
		{
			return !IsValid(Candidate) || !UMy_AuraAbilitySystemLibrary::IsNotFriend(SourceAvatar, Candidate);
		});
	}

	// int32 NumAdditionalTargets = FMath::Min(MaxNumShockTargets, GetAbilityLevel()-1);
	int32 NumAdditionalTargets = 5;

	UMy_AuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets,OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* TargetActor : OutAdditionalTargets)
	{
		if (IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(TargetActor))
		{
			FMy_DeathSignature& OnDeath = CombatInterface->GetOnDeath();          // ① 拿引用
			if (!OnDeath.IsAlreadyBound(this, &UMy_AuraBeamSpell::AdditionalTargetDie))   // ② 查重
			{
				OnDeath.AddDynamic(this, &UMy_AuraBeamSpell::AdditionalTargetDie);        // ③ 绑
			}
		}
	}
}
