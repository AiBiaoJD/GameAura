// Copyright ABiao

#include "My_Character/Enemy_Characte.h"
#include "DrawDebugHelpers.h"
#include "Aura/Aura.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void AEnemy_Characte::HighlightActor()
{
	/*
	 * Post Progress: Depth-Stencil一个单独的深度缓冲区，用于存储特定物体的深度信息。与主深度缓冲区不同，它可以独立于场景的其他部分进行处理。
	 * 当我们悬停鼠标到敌人,就启动深度缓冲区；没悬停就不启动深度缓冲区
	 * 启动后就可以设置CustomDepth Stencil Value来显示轮廓
	 * 当然这个轮廓是在Post Progress下面的Material中设置的
	 */

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

int32 AEnemy_Characte::GetPlayerLevel()
{
	return Level;
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

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AEnemy_Characte::BeginPlay()
{
	Super::BeginPlay();

	My_InitAbilityActorInfo();

	/*
	 * 血量UI WidgetController部分
	 */

	 // SetWidgetController,因为里面是对委托绑定（也可以后绑定,但绑定后要广播初始值,保证UI正确）
	if (UMy_AuraUserWidget* AuraUserWidget = CastChecked<UMy_AuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	// 初始化widgetController,里面是对委托的广播
	if (UMy_AuraAttributeSet* AuraAS = CastChecked<UMy_AuraAttributeSet>(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAS->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});

		// 广播初始值
		OnHealthChanged.Broadcast(AuraAS->GetHealth());
		OnMaxHealthChanged.Broadcast(AuraAS->GetMaxHealth());
	}

}

void AEnemy_Characte::My_InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttribute();


}
