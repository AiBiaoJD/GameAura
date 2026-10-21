// Copyright ABiao


#include "My_AbilityActor/My_MagicCircle.h"

#include "Components/DecalComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
AMy_MagicCircle::AMy_MagicCircle()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>("MagicCircleDecal");
	MagicCircleDecal->SetupAttachment(SceneRoot);
}

void AMy_MagicCircle::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMy_MagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

