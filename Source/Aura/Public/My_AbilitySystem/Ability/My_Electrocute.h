// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_AbilitySystem/Ability/My_AuraBeamSpell.h"
#include "My_Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_Electrocute : public UMy_AuraBeamSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 Level) override;
	virtual FString GetNextLevelDescription(int32 Level) override;
};
