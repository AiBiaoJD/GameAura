// Copyright ABiao


#include "My_Character/MyCharacter_Base.h"

// Sets default values
AMyCharacter_Base::AMyCharacter_Base()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMyCharacter_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

