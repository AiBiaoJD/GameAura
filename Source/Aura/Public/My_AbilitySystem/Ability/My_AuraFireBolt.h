// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Aura/Public/My_AbilitySystem/Ability/My_AuraProjectileSpell.h"
#include "My_AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraFireBolt : public UMy_AuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(blueprintCallable, Category = "My_Ability|FireBolt")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	float SpawnSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	float HomingAccelerationMax = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	float HomingMinDistance = 300.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	bool bLaunchHomingProjectile = true;
};
