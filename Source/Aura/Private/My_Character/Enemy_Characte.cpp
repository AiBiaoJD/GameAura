// Copyright ABiao

#include "My_Character/Enemy_Characte.h"
#include "DrawDebugHelpers.h"
#include "Aura/Aura.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void AEnemy_Characte::HighlightActor()
{
	// 启用 Custom Depth
	//允许开发者将特定对象渲染到一个单独的深度缓冲区中
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

}

void AEnemy_Characte::UnHighlightActor()
{
	// 不启用 Custom Depth
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);

}

AEnemy_Characte::AEnemy_Characte()
{
	/*
	ECC_Visibility 是 Unreal Engine 中的一个内置碰撞通道（Collision Channel），专门用于处理与 可见性Visibility） 相关的碰撞检测。
	它通常用于光线追踪（Line Trace）、鼠标点击检测等场景，用来检测对象是否可见或可交互。
	Visibility 通道 设置为 Block，射线会命中该对象，并返回对象信息
	*
	*/
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMy_AuraAbilitySystemComponent
	>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UMy_AuraAttributeSet>("AttributeSet");

}

void AEnemy_Characte::BeginPlay()
{
	Super::BeginPlay();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}
