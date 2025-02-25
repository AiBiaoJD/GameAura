// Copyright ABiao


#include "My_EffectActor/My_AuraEffectActor.h"

AMy_AuraEffectActor::AMy_AuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AMy_AuraEffectActor::OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//ASC定义了接口IAbilitySystemInterface，来获取Actor的ASC

}

void AMy_AuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMy_AuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMy_AuraEffectActor::OnOverLap);
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AMy_AuraEffectActor::EndOverlap);
}


