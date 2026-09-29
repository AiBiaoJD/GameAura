// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AuraAbilityTypes.h"
#include "NiagaraSystem.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "My_ProjectileActor.generated.h"

UCLASS()
class AURA_API AMy_ProjectileActor : public AActor
{
	GENERATED_BODY()

public:
	AMy_ProjectileActor();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// 暴露给蓝图的变量，并在生成时显示
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn = true))
	FMy_DamageEffectParams DamageEffectParams;

	// 【强引用成员】用来保住虚拟追踪点组件，防止GC回收
	// 当追踪地面时，新建的USceneComponent会存到这里，UPROPERTY标记提供强引用保护
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

	/* ==========================================================================================
	 * 追踪（Homing）数据 —— ★ 必须做成【复制属性】
	 * ------------------------------------------------------------------------------------------
	 * 为什么不能像原来那样，直接在 SpawnProjectiles 里设 ProjectileMovement：
	 *
	 *   ① UProjectileMovementComponent 【没有重写 GetLifetimeReplicatedProps】
	 *      → 它一个属性都不复制。bIsHomingProjectile / HomingAccelerationMagnitude /
	 *        HomingTargetComponent 全是【纯本地】的。
	 *
	 *   ② 而 SpawnProjectiles 开头就是 if (!HasAuthority()) return;
	 *      → 客户端那份投射物从来没执行过追踪设置 → bIsHomingProjectile 默认 false、
	 *        HomingTargetComponent 默认 nullptr → 客户端看到的是【一条直线】。
	 *
	 * 解决思路：不复制"组件指针"，而是复制"追踪所需要的信息"，
	 *           客户端收到后由 ApplyHomingSetup() 自己组装出同样的追踪。
	 * ========================================================================================== */

	/** 追踪目标（敌人）。为空则表示追踪下面的 HomingTargetLocation 那个地面点 */
	UPROPERTY(ReplicatedUsing = OnRep_HomingSetup)
	TObjectPtr<AActor> HomingTargetActor;

	/** 追踪的地面点（HomingTargetActor 为空时使用） */
	UPROPERTY(ReplicatedUsing = OnRep_HomingSetup)
	FVector HomingTargetLocation = FVector::ZeroVector;

	/** 是否开启追踪 */
	UPROPERTY(ReplicatedUsing = OnRep_HomingSetup)
	bool bHomingEnabled = false;

	/**
	 * ★ 服务器骰好的追踪加速度（FRandRange 的随机结果）。
	 *   客户端必须直接用这个值，【绝对不要】自己再骰一次 ——
	 *   否则两端随机结果不同，轨迹还是不一样。
	 */
	UPROPERTY(ReplicatedUsing = OnRep_HomingSetup)
	float HomingAcceleration = 0.f;

protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 复制回调：客户端收到追踪数据后组装 ProjectileMovement 的设置 */
	UFUNCTION()
	void OnRep_HomingSetup();

	/** 把上面那些信息组装成 ProjectileMovement 的追踪设置（服务器和客户端都会调） */
	void ApplyHomingSetup();

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayImpactEffects();

private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 8.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere, Category = "My_Impact")
	TObjectPtr<UNiagaraSystem> ImpactEffect; 

	UPROPERTY(EditAnywhere, Category = "My_Impact")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "My_Impact")
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
