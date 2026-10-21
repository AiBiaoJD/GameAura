// Copyright ABiao


#include "Aura/Public/My_AbilitySystem/Ability/My_AuraPassiveAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Aura/Public/My_AbilitySystem/My_AuraAbilitySystemComponent.h"

void UMy_AuraPassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (UMy_AuraAbilitySystemComponent* AuraASC = Cast<UMy_AuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo())))
	{
		AuraASC->OnDeactivatePassiveAbility.AddUObject(this, &UMy_AuraPassiveAbility::ReceiveDeactivate);
	}
}

void UMy_AuraPassiveAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (AbilityTags.HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}
