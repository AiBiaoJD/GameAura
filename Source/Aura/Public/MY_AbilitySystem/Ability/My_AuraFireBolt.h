// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Ability/My_AuraProjectileSpell.h"
#include "My_AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraFireBolt : public UMy_AuraProjectileSpell
{
	GENERATED_BODY()
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
