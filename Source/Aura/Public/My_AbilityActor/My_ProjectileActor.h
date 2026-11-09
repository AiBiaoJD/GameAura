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

	UPROPERTY(VisibleAnywhere, Category = "My_Projectile|Common|Movement")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// 暴露给蓝图的变量，并在生成时显示
	UPROPERTY(BlueprintReadWrite, Category = "My_Projectile|Common|Damage", meta=(ExposeOnSpawn = true))
	FMy_DamageEffectParams DamageEffectParams;

	/*
	 * 【强引用成员】用来保住"追踪地面点"时临时造的那个虚拟组件，防止被GC回收。
	 *
	 * ProjectileMovement->HomingTargetComponent 是 TWeakObjectPtr，不构成强引用；
	 * 不自己持有的话，追踪地面点时它会被回收 —— 表现为"飞一会儿突然不追踪了"。
	 */
	UPROPERTY()
	TObjectPtr<USceneComponent> HomingTargetSceneComponent;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Destroyed() override;

	/*
	 * 播放命中特效 + 音效（所有端）。
	 *
	 * ★ 只能在服务器调用。NetMulticast 从客户端发起会被丢弃，
	 *   日志报 "Calling Multicast function ... on client"，蓝图侧务必用 Has Authority 包住。
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void MulticastPlayImpactEffects();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "My_Projectile|Common|Collision")
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;


private:
	UPROPERTY(EditDefaultsOnly, Category = "My_Projectile|Common|Life")
	float LifeSpan = 8.f;

	UPROPERTY(EditAnywhere, Category = "My_Projectile|Common|Impact")
	TObjectPtr<UNiagaraSystem> ImpactEffect; 

	UPROPERTY(EditAnywhere, Category = "My_Projectile|Common|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	/*
	 * 爆炸音效的最小间隔（秒）。
	 *
	 * FireBlast 一次甩 12 颗球，它们几乎同一时刻回到身边爆炸，
	 * 12 个 PlaySoundAtLocation 叠加会把音量放大十几倍。
	 * 这个值控制「多久之内只允许响一声」：
	 *   0    → 不节流（每颗球都响）
	 *   0.1  → 大约连响 2~3 声
	 *   0.2  → 基本只响 1 声
	 */
	UPROPERTY(EditDefaultsOnly, Category = "My_Projectile|Common|Impact")
	float ImpactSoundCooldown = 0.15f;

	UPROPERTY(EditAnywhere, Category = "My_Projectile|Common|Impact")
	TObjectPtr<USoundBase> LoopingSound;
};
