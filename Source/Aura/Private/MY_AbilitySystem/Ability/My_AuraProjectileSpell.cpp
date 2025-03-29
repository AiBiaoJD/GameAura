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

void UMy_AuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// ProjectileActor需要在服务器生成,replicate到客户端
	if (!GetAvatarActorFromActorInfo()->HasAuthority()) return;

	/*
	 * 这里不直接获取AuraCharter,而是使用Interface结构
	 * 是为了让Ability不绑定Aura角色,可以检查任意的角色/敌人
	 */
	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface)
	{
		const FVector SockLoc = CombatInterface->GetWeaponSockLocation();
		FRotator Rotation = (ProjectileTargetLocation - SockLoc).Rotation();
		Rotation.Pitch = 0.f;

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
		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

		float DamageValue = Damage.GetValueAtLevel(15);
		SpecHandle.Data->SetSetByCallerMagnitude(FMy_AuraGameplayTags::GetInstance().My_EffectData_Damage, DamageValue);
		Projectile->DamageEffectSpecHandle = SpecHandle;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
