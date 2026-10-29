// Copyright ABiao


#include "My_AbilityActor/My_MagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AMy_MagicCircle::AMy_MagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	/*
	 * 纯本地视觉：每个客户端自己一份，不参与网络复制。
	 * 开着复制的话，服务器上那份会被同步到所有客户端，
	 * 别人施法你也能看到圆环，而且位置是服务器那份的（服务器没有鼠标，只会钉在原地）。
	 */
	bReplicates = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(SceneRoot);
}

void AMy_MagicCircle::SetMagicCircleMaterial(UMaterialInterface* DecalMaterial)
{
	if (IsValid(MagicCircleDecal) && DecalMaterial)
	{
		MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void AMy_MagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMy_MagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

