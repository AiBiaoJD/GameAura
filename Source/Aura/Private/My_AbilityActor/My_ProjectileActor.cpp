// Copyright ABiao


#include "My_AbilityActor/My_ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AMy_ProjectileActor::AMy_ProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_MyProjectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //仅启用查询碰撞检测
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AMy_ProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMy_ProjectileActor::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AMy_ProjectileActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
	 * 1.火球在PlayasCLient模式下,会与施法者发生碰撞,这是我们要避免的(客户端和服务器都要)
	 * 
	 *  方法：服务器DamageEffectSpecHandle.Data有效，客户端DamageEffectSpecHandle.Data无效
	 *   
	 *  我们要处理客户端和服务器火球与施法者碰撞问题，使用下面第2个if只能处理服务器。客户端解决不了。
	 *
	 * 
	 * 2.因为客户端和服务器都会触发MulticastRPC,如果我们不加第1个if,客户端会触发MulticastRPC，导致视觉上火球和施法者发生碰撞
	 *
	 * 方法：所有逻辑在服务器上实现，即添加第1个if。
	 * 这样会使MulticastRPC在Server调用,所有客户端同步显示特效。
	 * 并且也能解决客户端火球和施法者碰撞
	 * 
	 */
	if (!HasAuthority()) return;

	if (DamageEffectSpecHandle.Data.Get()->GetContext().GetEffectCauser() == OtherActor) return;

	MulticastPlayImpactEffects();

	// 激活Effect,只能在服务器修改Attribute,Replicate Attribute到客户端
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
	}
	Destroy();
}

void AMy_ProjectileActor::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void AMy_ProjectileActor::MulticastPlayImpactEffects_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}
