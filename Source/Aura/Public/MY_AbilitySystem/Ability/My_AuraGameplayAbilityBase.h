// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "My_AuraGameplayAbilityBase.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraGameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:

	//根据InputAction的Tag 和 StartUpInputTag 去激活能力
	UPROPERTY(EditAnywhere, Category = "My_Input")
	FGameplayTag StartUpInputTag;

};
