// Copyright ABiao

#include "My_Character/Enemy_Characte.h"
#include "DrawDebugHelpers.h"
#include "Aura/Aura.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

void AEnemy_Characte::HighlightActor()
{
	// ���� Custom Depth
	//�������߽��ض�������Ⱦ��һ����������Ȼ�������
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

}

void AEnemy_Characte::UnHighlightActor()
{
	// ������ Custom Depth
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);

}

AEnemy_Characte::AEnemy_Characte()
{
	/*
	ECC_Visibility �� Unreal Engine �е�һ��������ײͨ����Collision Channel����ר�����ڴ����� �ɼ���Visibility�� ��ص���ײ��⡣
	��ͨ�����ڹ���׷�٣�Line Trace������������ȳ����������������Ƿ�ɼ���ɽ�����
	Visibility ͨ�� ����Ϊ Block�����߻����иö��󣬲����ض�����Ϣ
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
