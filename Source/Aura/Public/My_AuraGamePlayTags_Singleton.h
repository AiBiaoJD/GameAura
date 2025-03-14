// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton Containing native Gameplay Tags
 */

struct FMy_AuraGameplayTags
{
public:
	static const FMy_AuraGameplayTags& GetInstance() { return GameplayTagsInstance; }
	static void InitializeNativeGameplayTags();

	FGameplayTag My_Attribute_Secondary_Armor;

protected:


private:
	static FMy_AuraGameplayTags GameplayTagsInstance;
};
