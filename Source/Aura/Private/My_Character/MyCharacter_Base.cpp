// Copyright ABiao


#include "My_Character/MyCharacter_Base.h"

// Sets default values
AMyCharacter_Base::AMyCharacter_Base()
{
	bReplicates = true;
	bNetLoadOnClient = true;

	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* AMyCharacter_Base::GetAbilitySystemComponent() const
{
	return  AbilitySystemComponent;
}

UAttributeSet* AMyCharacter_Base::GetAttributeSet() const
{
	return AttributeSet;
}

// Called when the game starts or when spawned
void AMyCharacter_Base::BeginPlay()
{
	Super::BeginPlay();

}

void AMyCharacter_Base::My_InitAbilityActorInfo()
{
}


void AMyCharacter_Base::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GamePlayEffect, float Level)const
{
	/*check(GetAbilitySystemComponent());
	check(GamePlayEffect);*/
	if (GetAbilitySystemComponent() && GamePlayEffect)
	{
		const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();

		const FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GamePlayEffect, Level, ContextHandle);

		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
	}

}

void AMyCharacter_Base::InitializeDefaultAttribute()const
{
	ApplyEffectToSelf(DefaultPrimaryAttributeEffectClass, 1.0f);
	ApplyEffectToSelf(DefaultSecondAttributeEffectClass, 1.0f);

}

