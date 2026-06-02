// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraDamageGameplayAbility.h"
#include "My_AbilityActor/My_ProjectileActor.h"
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
	virtual void  ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable, Category="My_Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMy_ProjectileActor> ProjectileClass;


};