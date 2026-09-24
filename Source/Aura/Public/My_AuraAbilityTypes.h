#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "My_AuraAbilityTypes.generated.h"

// ★ ApplyGE 之前的「参数原料箱」
//   填：GA 侧从蓝图配好值（MakeDamageEffectParamsFromClassDefaults）
//   存：投射物的 UPROPERTY 成员，跨越飞行时间
//   用：命中时交给 ApplyDamageEffect 分流到【三个地方】——
//       ① 造 Spec 的原料：DamageGameplayEffectClass / AbilityLevel
//       ② 调用对象：      SourceASC（谁造）/ TargetASC（谁挨打）
//       ③ SetByCaller（float 数值，给 ExecCalc 读）：
//            BaseDamage / DamageType / DebuffChance/Damage/Duration/Frequency
//       ④ Context（非 float，给 ExecCalc / Post 读）：
//            DeathImpulse（死亡冲量，FVector 塞不进 SetByCaller）
USTRUCT(BlueprintType)
struct FMy_DamageEffectParams
{
	GENERATED_BODY()

	FMy_DamageEffectParams()
	{
	}

	UPROPERTY()
	TObjectPtr<UObject> WorldContextObject = nullptr;
	UPROPERTY()
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> SourceASC;
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> TargetASC;

	UPROPERTY()
	float BaseDamage = 0.f;
	UPROPERTY()
	float AbilityLevel = 1.f;
	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	float DebuffChance = 0.f;
	UPROPERTY()
	float DebuffDamage = 0.f;
	UPROPERTY()
	float DebuffFrequency = 0.f;
	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	float DeathImpulseMagnitude = 0.f;
	UPROPERTY()
	FVector DeathImpulse = FVector(0.f, 0.f, 0.f);
};

USTRUCT(BlueprintType)
struct FMY_AuraGamePlayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsCriticalHit() const { return bIsCriticalHit; };
	bool IsBlockedHit() const { return bIsBlockedHit; };
	bool IsSuccessfulDebuff() const { return bIsSuccessfulDebuff; };
	float GetDebuffDamage() const { return DebuffDamage; };
	float GetDebuffFrequency() const { return DebuffFrequency; };
	float GetDebuffDuration() const { return DebuffDuration; };
	FGameplayTag GetDamageType() const { return DamageType; };
	FVector GetDeathImpulse() const { return DeathImpulse; };

	void SetIsCriticalHit(bool bInCriticalHit) { bIsCriticalHit = bInCriticalHit; }
	void SetIsBlockedHit(bool bInBlockedHit) { bIsBlockedHit = bInBlockedHit; }
	void SetIsSuccessfulDebuff(bool bInSuccessfulDebuff) { bIsSuccessfulDebuff = bInSuccessfulDebuff; }
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; };
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; };
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; };
	void SetDamageType(const FGameplayTag& InDamageType) { DamageType = InDamageType; };
	void SetDeathImpulse(FVector InDeathImpulse) { DeathImpulse = InDeathImpulse; };

	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FMY_AuraGamePlayEffectContext* Duplicate() const
	{
		FMY_AuraGamePlayEffectContext* NewContext = new FMY_AuraGamePlayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

protected:
	UPROPERTY()
	bool bIsBlockedHit = false;

	UPROPERTY()
	bool bIsCriticalHit = false;

	UPROPERTY()
	bool bIsSuccessfulDebuff = false;

	UPROPERTY()
	float DebuffDamage = 0.f;

	UPROPERTY()
	float DebuffFrequency = 0.f;

	UPROPERTY()
	float DebuffDuration = 0.f;

	UPROPERTY()
	FGameplayTag DamageType = FGameplayTag();

	UPROPERTY()
	FVector DeathImpulse = FVector(0.f, 0.f, 0.f);
};

template <>
struct TStructOpsTypeTraits<FMY_AuraGamePlayEffectContext> : public TStructOpsTypeTraitsBase2<FMY_AuraGamePlayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true // Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
