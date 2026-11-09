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

	/*
	 * 当前等级实际发几颗火球。
	 *   配了曲线 → 完全按曲线取
	 *   没配曲线 → NumFireBalls.Value + FireBallsPerLevel × (Level - 1)
	 * 最后统一夹到 [1, MaxNumFireBalls]。
	 */
	UFUNCTION(BlueprintPure, Category = "My_Ability|FireBlast")
	int32 GetNumFireBallsAtLevel(int32 Level) const;

protected:
	/*
	 * 火球数量（等级 1 发几颗），360° 均分，每颗占 360/Count 度。
	 *
	 * 8 起步是倒推出来的：ExplosionOuterRadius = 100、ExplodeDistance = 100 时
	 *   8 颗 → 球心间距 76.5，outer/间距 ≈ 1.31
	 *  12 颗 → 球心间距 51.8，outer/间距 ≈ 1.93
	 * 这个 1.3~1.9 的区间正好：相邻球有重叠、方向之间不留缝，又不会一圈球全盖住同一个敌人。
	 * 数量再少（≤6，间距 ≥100）单球范围就开始互相「抢」同一个目标；再多（>12）球心过密，
	 * 一个敌人被反复覆盖，爆炸伤害会失控 —— 所以有下面的 MaxNumFireBalls 兜着。
	 *
	 * 随等级变多有两种写法：
	 *   ① 配曲线：CT_Damage 里加一行 FireBlast.NumFireBalls，
	 *      等级 1~5 填 8/9/10/11/12，此时 Value 是乘数，一般留 1
	 *   ② 不配曲线：走下面的 FireBallsPerLevel 线性增长（不用做任何资产）
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	FScalableFloat NumFireBalls = FScalableFloat(8.f);

	/*
	 * 曲线留空时每升一级额外多几颗。默认 8 + 1/级 → 等级 5 发 12 颗。
	 * ★ 配了曲线就忽略这个字段。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	int32 FireBallsPerLevel = 1;

	/*
	 * 火球数量上限（曲线和线性增长都受它约束）。
	 * 12 是配合 ExplosionOuterRadius = 100 的上限，见上面的推导；
	 * 真要发更多，得同时放大 ExplosionOuterRadius，否则球心过密、伤害会叠爆。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	int32 MaxNumFireBalls = 12;

	// 火球飞出的水平距离
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float FireBallDistance = 400.f;

	// 火球飞出时抬升的高度（ApexLocation 的 Z 偏移）
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float FireBallHeight = 20.f;

	/*
	 * 爆炸范围（单颗球的）。
	 *
	 * 默认值是按「ExplodeDistance = 100 + 8~12 颗球」算出来的：
	 * 火球爆炸时分布在施法者周围半径 100 的圆上，相邻球心距离 76.5（8 颗）~ 51.8（12 颗）。
	 * 外半径 100 的效果是：正对火球方向的敌人吃满伤，隔一个球心衰减到零头，
	 * 对径方向完全够不到 —— 也就是每个方向各管一片，不会一圈球全中同一个敌人。
	 *
	 * ★ 火球数量随等级变，球心间距会跟着变，改数量后要回来核对这两个值：
	 *   球心间距 = 2 × ExplodeDistance × sin(180° / N)
	 *   ExplodeDistance = 100 时：N=6 → 100、N=8 → 76.5、N=10 → 61.8、N=12 → 51.8
	 *   外半径取「球心间距的 1.3~2 倍」比较合适。
	 *
	 * ★ 另一个副作用：球越多，同一个敌人被覆盖到的次数越多，
	 *   爆炸总伤害 ≈ 单颗伤害 × 命中球数，等于数量本身也在放大伤害。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float ExplosionInnerRadius = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	float ExplosionOuterRadius = 100.f;

	/*
	 * 爆炸伤害（径向），和基类的 Damage（火球撞人）分开配。
	 *
	 * 两者都是 FScalableFloat，本来就按等级取值 —— 要让爆炸伤害单独随等级走，
	 * 在 CT_Damage 里给它自己配一行（如 FireBlast.ExplosionDamage）；
	 * 曲线留空且 Value 为 0 时回落到基类的 Damage，此时两者数值完全一样。
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	FScalableFloat ExplosionDamage;

private:
	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|FireBlast")
	TSubclassOf<AMy_FireBall> FireBallClass;
};
