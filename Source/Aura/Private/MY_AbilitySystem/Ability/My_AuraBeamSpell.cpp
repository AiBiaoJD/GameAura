// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraBeamSpell.h"

#include "GameFramework/Character.h"

void UMy_AuraBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		CancelAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true);
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
