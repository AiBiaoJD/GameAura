// Copyright ABiao


#include "My_EffectActor/My_AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

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
	if (!OtherActor || !OtherComp)
	{
		return;
	}
	//ASC定义了接口IAbilitySystemInterface，来获取Actor的ASC
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
		if (ASC)
		{
			const UMy_AuraAttributeSet* AuraAttributeSet = Cast<UMy_AuraAttributeSet>(ASC->GetAttributeSet(UMy_AuraAttributeSet::StaticClass()));
			if (AuraAttributeSet)
			{
				//强制转换为非const，为了后面的sethealth
				//todo:不建议怎么做后面进行修改
				UMy_AuraAttributeSet* MuteAttributesSet = const_cast<UMy_AuraAttributeSet*>(AuraAttributeSet);
				MuteAttributesSet->SetHealth(AuraAttributeSet->GetHealth() + 50.0f);

				Destroy();
			}
		}
	}

}

void AMy_AuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AMy_AuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AMy_AuraEffectActor::OnOverLap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AMy_AuraEffectActor::EndOverlap);
}


