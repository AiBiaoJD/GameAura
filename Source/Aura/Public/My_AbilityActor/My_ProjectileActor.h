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
