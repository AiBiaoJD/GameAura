#pragma once
#include "GameplayEffectTypes.h"
#include "My_AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FMY_AuraGamePlayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()
public:
	bool IsCriticalHit() const { return bIsCriticalHit; };
	bool IsBlockedHit() const { return bIsBlockedHit; };
	void SetIsCriticalHit(bool bInCriticalHit) { bIsCriticalHit = bInCriticalHit; }
	void SetIsBlockedHit(bool bInBlockedHit) { bIsBlockedHit = bInBlockedHit; }

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
};

// 为 FGameplayEffectContext 结构体定义 类型特性（Type Traits）
template<>
struct TStructOpsTypeTraits< FMY_AuraGamePlayEffectContext > : public TStructOpsTypeTraitsBase2< FMY_AuraGamePlayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};
