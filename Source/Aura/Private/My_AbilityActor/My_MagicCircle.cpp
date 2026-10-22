// Copyright ABiao


#include "My_AbilityActor/My_MagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"
#include "Materials/MaterialInterface.h"

// Sets default values
AMy_MagicCircle::AMy_MagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

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

