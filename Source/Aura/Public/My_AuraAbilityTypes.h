#pragma once
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "My_AuraAbilityTypes.generated.h"

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
	
	void SetIsCriticalHit(bool bInCriticalHit) { bIsCriticalHit = bInCriticalHit; }
	void SetIsBlockedHit(bool bInBlockedHit) { bIsBlockedHit = bInBlockedHit; }
	void SetIsSuccessfulDebuff(bool bInSuccessfulDebuff) { bIsSuccessfulDebuff = bInSuccessfulDebuff; }
	void SetDebuffDamage(float InDamage) { DebuffDamage = InDamage; };
	void SetDebuffFrequency(float InFrequency) { DebuffFrequency = InFrequency; };
	void SetDebuffDuration(float InDuration) { DebuffDuration = InDuration; };
	void SetDamageType(const FGameplayTag& InDamageType) { DamageType = InDamageType; };

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
