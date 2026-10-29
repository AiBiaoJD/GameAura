// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraAbilityTypes.h"
#include "Aura/Public/My_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
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

	// InRadialDamageOrigin：径向伤害的球心。每次施法都不同（角色位置 / 命中点 / 法阵位置），
	// 所以是参数不是 EditDefaultsOnly 成员；传 ZeroVector 时回退到施法者脚下。
	UFUNCTION(BlueprintPure)
	FMy_DamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr,
	                                                               FVector InRadialDamageOrigin = FVector::ZeroVector) const;

	UFUNCTION(BlueprintPure)
	float GetDamageAtLevel();

protected:
	// ===== 基础伤害 =====
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My_Ability|Common|Damage")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Damage")
	FGameplayTag DamageType;
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Damage")
	FScalableFloat Damage;

	// ===== Debuff（每次命中掷骰，决定要不要挂持续伤害）=====
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Debuff")
	float DebuffChance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Debuff")
	float DebuffDamage = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Debuff")
	float DebuffFrequency = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Debuff")
	float DebuffDuration = 5.f;

	// ===== 冲量（死亡打飞 / 击退）=====
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Impulse")
	float DeathImpulseMagnitude = 14000.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Impulse")
	float KnockbackChance = 20.f;
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Common|Impulse")
	float KnockbackMagnitude = 600.f;

	// ===== 径向伤害（AOE：按目标到球心的距离衰减）=====
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My_Ability|Common|Radial")
	bool bIsRadialDamage = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My_Ability|Common|Radial")
	float RadialDamageInnerRadius = 0.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "My_Ability|Common|Radial")
	float RadialDamageOuterRadius = 0.f;
};
