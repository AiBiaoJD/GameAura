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
