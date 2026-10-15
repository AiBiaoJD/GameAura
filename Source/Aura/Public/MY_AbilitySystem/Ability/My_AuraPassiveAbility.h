// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "My_AuraPassiveAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraPassiveAbility : public UMy_AuraGameplayAbilityBase
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
