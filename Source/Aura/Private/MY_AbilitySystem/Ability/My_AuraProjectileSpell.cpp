// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraProjectileSpell.h"

#include "Kismet/KismetSystemLibrary.h"

void UMy_AuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UKismetSystemLibrary::PrintString(this,FString("ActivateAbility C++"),true,true,FLinearColor::Blue,2.f);
}
