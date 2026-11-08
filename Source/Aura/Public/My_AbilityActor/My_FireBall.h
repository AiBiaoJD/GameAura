// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AbilityActor/My_ProjectileActor.h"
#include "My_FireBall.generated.h"

/**
 * 火焰爆裂的火球：从施法者身边甩出，飞到 ApexLocation 后掉头飞回，
 * 回程撞击敌人造成直接伤害，贴近施法者时爆炸造成径向伤害。
 * 位移由蓝图里的两条 Timeline 驱动，C++ 只提供数据与碰撞判定。
 */
UCLASS()
class AURA_API AMy_FireBall : public AMy_ProjectileActor
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 蓝图在 BeginPlay 调用，启动「向外飞」的 Timeline
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	// 「飞回来」的 Timeline，由蓝图在上一条播完后启动
	UFUNCTION(BlueprintImplementableEvent)
	void StartReturningTimeline();

	// 回程追踪目标（施法者）
	UPROPERTY(BlueprintReadOnly, Replicated)
	TObjectPtr<AActor> ReturnToActor;

	/*
	 * 最远点，蓝图拿它做 VLerp 的终点。
	 *
	 * 必须复制：这个值只在服务器的 SpawnFireBalls 里算得出来，
	 * 客户端拿不到就会退化成 (0,0,0) —— 12 颗球全部朝世界原点插值，
	 * 路径完全重合，看起来只剩 1 颗，而且永远回不到角色身上。
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ApexLocation)
	FVector ApexLocation = FVector::ZeroVector;

	/*
	 * 回到施法者身边时的径向爆炸参数。
	 *
	 * 不复制：伤害是服务器权威的，客户端只需要播爆炸特效（用位置就够了）。
	 * 如果复制过去，客户端会真的拿着半径去取目标、再 ApplyDamageEffect，
	 * 非权威端应用 GE 会造成本地属性抖动（敌人血条闪一下再被复制修正）。
	 * 蓝图里那段伤害循环记得用 HasAuthority 包住。
	 */
	UPROPERTY(BlueprintReadWrite)
	FMy_DamageEffectParams ExplosionDamageParams;

protected:
	virtual void BeginPlay() override;

	// 客户端收到最远点后才开始播 Timeline，避免用默认值起跑
	UFUNCTION()
	void OnRep_ApexLocation();

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	// 距施法者多近判定为「回到身边」并触发爆炸
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "My_Projectile|FireBall")
	float ExplodeDistance = 100.f;
};
