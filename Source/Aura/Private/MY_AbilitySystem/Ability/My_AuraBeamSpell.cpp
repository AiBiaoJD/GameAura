// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraBeamSpell.h"

#include "My_AuraGamePlayTags_Singleton.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "My_Interraction/My_CombatInterface.h"

void UMy_AuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UMy_AuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo && CurrentActorInfo->PlayerController.IsValid())
	{
		OwnerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UMy_AuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	if (!OwnerCharacter) return;

	if (OwnerCharacter->Implements<UMy_CombatInterface>())
	{
		const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();
		const FVector SocketLocation = IMy_CombatInterface::Execute_GetWeaponSockLocation(OwnerCharacter, GameplayTags.My_CombatSocket_Weapon);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(OwnerCharacter);

		UKismetSystemLibrary::SphereTraceSingle(
			this,
			SocketLocation,
			BeamTargetLocation,
			10.f,
			TraceTypeQuery1,
			false,
			IgnoreActors,
			EDrawDebugTrace::ForDuration,
			HitResult,
			true
		);

		if (HitResult.bBlockingHit)
		{
			//命中目标逻辑
			MouseHitLocation = HitResult.ImpactPoint;
			MouseHitActor = HitResult.GetActor();
		}
	}
}
