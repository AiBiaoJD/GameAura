// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"
#include "My_AuraFireBlast.generated.h"

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

protected:
	// 火球数量，360° 均分，每颗占 360/NumFireBalls 度
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	int32 NumFireBalls = 12;
};
