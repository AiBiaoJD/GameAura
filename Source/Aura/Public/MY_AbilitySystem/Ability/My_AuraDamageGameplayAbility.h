// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
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

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
