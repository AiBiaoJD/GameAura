// Copyright ABiao


#include "My_AbilityActor/My_ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"

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

/* ==========================================================================================
 * 追踪（Homing）的网络同步
 * ------------------------------------------------------------------------------------------
 * 【问题】UProjectileMovementComponent 没有重写 GetLifetimeReplicatedProps
 *         → 它一个属性都不复制。而追踪设置只在服务器的 SpawnProjectiles 里做过
 *           （那个函数开头就 if (!HasAuthority()) return;）
 *         → 客户端那份投射物 bIsHomingProjectile = false、HomingTargetComponent = nullptr
 *           → 客户端看到火球【直线飞】，而服务器上它是拐弯命中的。
 *
 * 【解决】复制"追踪所需要的信息"（在投射物上），客户端收到后自己组装出同样的追踪。
 * ========================================================================================== */
void AMy_ProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMy_ProjectileActor, HomingTargetActor);
	DOREPLIFETIME(AMy_ProjectileActor, HomingTargetLocation);
	DOREPLIFETIME(AMy_ProjectileActor, bHomingEnabled);
	DOREPLIFETIME(AMy_ProjectileActor, HomingAcceleration);
}

void AMy_ProjectileActor::OnRep_HomingSetup()
{
	// 客户端收到/更新追踪数据 → 重新组装一次
	ApplyHomingSetup();
}

/**
 * 把复制过来的追踪信息组装成 ProjectileMovement 的设置。
 * 服务器在 BeginPlay 时用初始值调一次；客户端靠 BeginPlay + OnRep_HomingSetup 调。
 * 这个函数是幂等的，重复调用没关系。
 */
void AMy_ProjectileActor::ApplyHomingSetup()
{
	if (!ProjectileMovement) return;

	USceneComponent* TargetComp = nullptr;

	if (IsValid(HomingTargetActor))
	{
		// ① 目标是敌人 Actor → 直接用它的根组件
		TargetComp = HomingTargetActor->GetRootComponent();
	}
	else if (!HomingTargetLocation.IsNearlyZero())
	{
		// ② 目标是地面上的一个点 → 自己造一个虚拟组件放在那里当追踪点
		//    ★ 必须存进 HomingTargetSceneComponent（UPROPERTY 强引用）保住它，
		//      因为 ProjectileMovement->HomingTargetComponent 是 TWeakObjectPtr，
		//      不构成强引用 —— 不自己持有就会被 GC 回收，表现为"飞一会儿突然不追踪了"。
		if (!HomingTargetSceneComponent)
		{
			HomingTargetSceneComponent = NewObject<USceneComponent>(this);
		}
		HomingTargetSceneComponent->SetWorldLocation(HomingTargetLocation);
		TargetComp = HomingTargetSceneComponent;
	}

	ProjectileMovement->HomingTargetComponent = TargetComp;
	// 没有有效目标就不要开追踪，否则 ProjectileMovement 会每帧去追一个空指针
	ProjectileMovement->bIsHomingProjectile = bHomingEnabled && (TargetComp != nullptr);
	// ★ 用服务器复制过来的值，客户端不要自己 FRandRange 重骰
	ProjectileMovement->HomingAccelerationMagnitude = HomingAcceleration;
}

void AMy_ProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMy_ProjectileActor::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());

	// 服务器：用 SpawnProjectiles 在 FinishSpawning 之前设好的值组装
	// 客户端：PostNetInit() 会先把复制的属性应用上，再派发 BeginPlay → 这里也能拿到正确数据
	ApplyHomingSetup();
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
	// 碰到施法者
	if (DamageEffectParams.SourceASC->GetAvatarActor() == OtherActor) return;
	// 碰到友军？
	if (!UMy_AuraAbilitySystemLibrary::IsNotFriend(DamageEffectParams.SourceASC->GetAvatarActor(), OtherActor)) return;

	// 碰到敌人！
	MulticastPlayImpactEffects();

	// 激活Effect,只能在服务器修改Attribute,Replicate Attribute到客户端
	if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
	{
		const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
		DamageEffectParams.DeathImpulse = DeathImpulse;

		const bool bKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
		if (bKnockback)
		{
			FRotator Rotation = GetActorRotation();
			Rotation.Pitch = 45.f;
			const FVector KnockDir = Rotation.Vector() * DamageEffectParams.KnockbackMagnitude;
			DamageEffectParams.Knockback = KnockDir;
		}
		else
		{
			// ★ 兜底清零：参数生成阶段可能会无条件写过一个默认值，
			//   掷骰失败时必须清掉，否则这里会变成"必击退"
			DamageEffectParams.Knockback = FVector::ZeroVector;
		}

		DamageEffectParams.TargetASC = TargetASC;

		UMy_AuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	}

	// 先关碰撞再销毁，防止同帧多次触发伤害
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Destroy();
}

void AMy_ProjectileActor::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	Super::Destroyed();

	// 主动切断强引用
	HomingTargetSceneComponent = nullptr;
}

void AMy_ProjectileActor::MulticastPlayImpactEffects_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}
