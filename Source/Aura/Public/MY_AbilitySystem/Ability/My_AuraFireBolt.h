// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Ability/My_AuraProjectileSpell.h"
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

	UFUNCTION(blueprintCallable, Category="My_FireBolt")
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget);

protected:
	UPROPERTY(EditDefaultsOnly, Category="My_FireBolt")
	float SpawnSpread = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="My_FireBolt")
	float HomingAccelerationMin = 1600.f;

	UPROPERTY(EditDefaultsOnly, Category="My_FireBolt")
	float HomingAccelerationMax = 3200.f;

	UPROPERTY(EditDefaultsOnly, Category="My_FireBolt")
	float HomingMinDistance = 300.f;

	UPROPERTY(EditDefaultsOnly, Category="My_FireBolt")
	bool bLaunchHomingProjectile = true;
};
