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

	// 玩家火球
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

	// 每颗火球追踪点在目标处的侧向偏移半径。
	// 0 = 全部收敛到同一点（会合并成一颗），越大扇形保持得越明显。
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	float HomingSpreadRadius = 80.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBolt")
	bool bLaunchHomingProjectile = true;
};
