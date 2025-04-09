// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "NiagaraComponent.h"
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
	
	// 服务器通知客户端销毁预测投射物的RPC
	UFUNCTION(Client, Reliable)
	void ClientDestroyPredictedProjectile();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// 暴露给蓝图的变量，并在生成时显示
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	UPROPERTY(Replicated)
	bool bIsPredicted = false; // 标记是否为预测火球

	UPROPERTY(Replicated)
	float SpawnTimestamp = 0.f;



protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayImpactEffects();

private:
	void PlayLocalImpactEffects();

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	UPROPERTY(VisibleAnywhere)
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
