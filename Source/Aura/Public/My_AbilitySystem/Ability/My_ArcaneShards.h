// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"
#include "My_ArcaneShards.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_ArcaneShards : public UMy_AuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
