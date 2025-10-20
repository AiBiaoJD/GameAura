// Copyright ABiao


#include "My_Character/MyCharacter_Base.h"

#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

// Sets default values
AMyCharacter_Base::AMyCharacter_Base()
{
	bReplicates = true;
	bNetLoadOnClient = true;
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetCollisionResponseToChannel(ECC_MyProjectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false); //确保mesh 和 capsule只有一个用来处理OnOverlap
}



UAbilitySystemComponent* AMyCharacter_Base::GetAbilitySystemComponent() const
{
	return  AbilitySystemComponent;
}

UAttributeSet* AMyCharacter_Base::GetAttributeSet() const
{
	return AttributeSet;
}

UAnimMontage* AMyCharacter_Base::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AMyCharacter_Base::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

void AMyCharacter_Base::MulticastHandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Dissolve();
}

// Called when the game starts or when spawned
void AMyCharacter_Base::BeginPlay()
{
	Super::BeginPlay();

}

FVector AMyCharacter_Base::GetWeaponSockLocation_Implementation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSockName);
}


void AMyCharacter_Base::My_InitAbilityActorInfo()
{
}


void AMyCharacter_Base::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GamePlayEffect, float Level) const
{
	if (GetAbilitySystemComponent() && GamePlayEffect)
	{
		FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle EffectSpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GamePlayEffect, Level, ContextHandle);

		GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*EffectSpecHandle.Data.Get(), GetAbilitySystemComponent());
	}
}

void AMyCharacter_Base::InitializeDefaultAttribute()const
{
	ApplyEffectToSelf(DefaultPrimaryAttributeEffectClass, 1.0f);
	ApplyEffectToSelf(DefaultSecondAttributeEffectClass, 1.0f);
	ApplyEffectToSelf(InitVitalAttributeEffectClass, 1.0f);
}

void AMyCharacter_Base::AddCharacterAbilities()
{
	// Ability添加到服务器,replicate到客户端
	if (!HasAuthority()) return;

	UMy_AuraAbilitySystemComponent* ASC = CastChecked<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent);

	ASC->AddCharacterAbilitiesFromASC(StartupAbility);

}

void AMyCharacter_Base::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMaterial);
		StartDissolveTimeline(DynamicMaterial);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMaterial);
		StartWeaponDissolveTimeline(DynamicMaterial);
	}
}

