// Copyright ABiao


#include "My_AbilityActor/My_ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Aura/Public/My_AbilitySystem/My_AuraAbilitySystemLibrary.h"

AMy_ProjectileActor::AMy_ProjectileActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	/*
	 * 打开"移动复制"。放在构造函数而不是 BeginPlay，有两个原因：
	 *   1. CDO 里就是 true，蓝图 Details 的 Replicate Movement 勾选能如实反映。
	 *      之前写在 BeginPlay，等于把蓝图勾选架空了 —— 勾不勾运行时都是 true。
	 *   2. 复制从 Actor 出生那一刻就生效，不用等 BeginPlay 才打开。
	 *
	 * UProjectileMovementComponent 一个属性都不复制（bIsHomingProjectile /
	 * HomingTargetComponent / HomingAccelerationMagnitude 全是纯本地的），
	 * 而追踪只在服务器的 SpawnProjectiles 里设置（那边开头就 HasAuthority 返回）。
	 *
	 * 打开这个之后，服务器算出来的位置和速度会随 FRepMovement 复制到客户端
	 * （引擎 Actor.cpp: DOREPLIFETIME_ACTIVE_OVERRIDE_FAST(AActor, ReplicatedMovement, ...)）。
	 * 客户端靠 AActor::PostNetReceiveVelocity 把 LinearVelocity 塞回
	 * ProjectileMovement->Velocity，自己积分就行 —— 不需要知道追踪目标是谁。
	 *
	 * 注意：bReplicateMovement 是 AActor 的 private 位域，子类不能直接赋值。
	 * 这里用非虚的 SetReplicatingMovement 而不是 SetReplicateMovement ——
	 * 后者在 UE5.2 是 virtual（Actor.h:588），构造函数里调会触发
	 * "在编译时解析了对构造函数内虚拟函数的调用"警告；
	 * 而 SetReplicatingMovement 只写位、没有副作用，正是为这种场景准备的。
	 */
	SetReplicatingMovement(true);

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
}



void AMy_ProjectileActor::MulticastPlayImpactEffects_Implementation()
{
	// 特效不节流：12 个爆炸圈叠在一起视觉上更饱满，也不会吵
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	/*
	 * 音效节流。
	 *
	 * FireBlast 一次甩 12 颗球，它们几乎同一时刻回到身边爆炸，
	 * 12 个 PlaySoundAtLocation 叠起来音量会翻十几倍，听起来像炸雷。
	 *
	 * 按 World 记录上次播放时间，ImpactSoundCooldown 秒内只播一次。
	 * 用 World 做 key 而不是直接写 static double：PIE 多客户端跑在同一个进程里，
	 * 裸静态变量会让几个客户端互相节流，导致只有一边有声音。
	 */
	static TMap<TWeakObjectPtr<UWorld>, double> LastImpactSoundTimes;

	UWorld* World = GetWorld();
	if (!World) return;

	double& LastTime = LastImpactSoundTimes.FindOrAdd(World);
	const double Now = World->GetTimeSeconds();

	if (Now - LastTime < ImpactSoundCooldown) return;

	LastTime = Now;
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
}
