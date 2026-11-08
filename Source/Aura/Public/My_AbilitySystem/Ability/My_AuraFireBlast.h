// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"
#include "My_AuraFireBlast.generated.h"

class AMy_FireBall;
/**
 * 火焰爆裂：以自身为中心向四周甩出多个火球，火球先向外飞出，
 * 再掉头飞回施法者，回程撞击敌人造成直接伤害，贴近自身时爆炸造成径向火焰伤害。
 */
UCLASS()
class AURA_API UMy_AuraFireBlast : public UMy_AuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;

	UFUNCTION(BlueprintCallable)
	TArray<AMy_FireBall*> SpawnFireBalls();
protected:
	// 火球数量，360° 均分，每颗占 360/NumFireBalls 度
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	int32 NumFireBalls = 12;

	// 火球飞出的水平距离
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float FireBallDistance = 400.f;

	// 火球飞出时抬升的高度（ApexLocation 的 Z 偏移）
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float FireBallHeight = 150.f;

	// 回到施法者身边时爆炸的径向伤害范围
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float ExplosionInnerRadius = 150.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float ExplosionOuterRadius = 300.f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	TSubclassOf<AMy_FireBall> FireBallClass;
};
