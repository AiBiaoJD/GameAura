// Copyright ABiao


#include "My_AbilityActor/My_ProjectileActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
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

void AMy_ProjectileActor::ClientDestroyPredictedProjectile_Implementation()
{
	if (bIsPredicted && IsValid(this))
	{
		Destroy();
	}
}


void AMy_ProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	// 预测投射物最多存在0.5秒（避免与服务器投射物共存）
	if (bIsPredicted) SetLifeSpan(1.f);
	else SetLifeSpan(LifeSpan);
	SpawnTimestamp = GetWorld()->GetTimeSeconds(); // 记录生成时间
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMy_ProjectileActor::OnSphereOverlap);
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	
}

void AMy_ProjectileActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMy_ProjectileActor, bIsPredicted);
	DOREPLIFETIME(AMy_ProjectileActor, SpawnTimestamp);
}

void AMy_ProjectileActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 客户端预测投射物：仅播放特效，不处理伤害
	if (!HasAuthority() && bIsPredicted)
	{
		PlayLocalImpactEffects();
		Destroy(); // 立即销毁预测投射物
		return;
	}

	// 服务器处理伤害和同步
	if (HasAuthority())
	{
		MulticastPlayImpactEffects(); // 同步特效到所有客户端

		// 应用伤害（仅服务器）

		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy(); // 服务器销毁投射物
	}
}

void AMy_ProjectileActor::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void AMy_ProjectileActor::PlayLocalImpactEffects()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}


void AMy_ProjectileActor::MulticastPlayImpactEffects_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
}
