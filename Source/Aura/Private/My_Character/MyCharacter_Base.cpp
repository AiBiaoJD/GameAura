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

void AMyCharacter_Base:: My_InitAbilityActorInfo()
{
}

