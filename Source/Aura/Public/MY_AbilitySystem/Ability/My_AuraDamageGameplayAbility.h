// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraAbilityTypes.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "My_AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraDamageGameplayAbility : public UMy_AuraGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	FMy_DamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DebuffDuration = 5.f;
};
