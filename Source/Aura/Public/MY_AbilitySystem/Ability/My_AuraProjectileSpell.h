// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraDamageGameplayAbility.h"
#include "My_AbilityActor/My_ProjectileActor.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "My_AuraProjectileSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraProjectileSpell : public UMy_AuraDamageGameplayAbility
{
	GENERATED_BODY()

protected:
	// Ability 被激活后的入口点
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="My_Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	// 修改 ServerSpawnProjectile 声明
	UFUNCTION(Server, Reliable)
	void ServerSpawnProjectile(const FTransform& SpawnTransform, const FVector& TargetLocation, float ClientTimestamp);

	void SpawnProjectileInternal(const FTransform& SpawnTransform, const FVector& TargetLocation, float ClientTimestamp);

	// 保存客户端生成的预测火球（时间戳为键）
	UPROPERTY()
	TMap<float, AMy_ProjectileActor*> ClientPredictedProjectiles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<AMy_ProjectileActor> ProjectileClass;
};
