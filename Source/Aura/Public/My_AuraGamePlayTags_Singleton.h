// Copyright ABiao

#pragma once

#include "CoreMinimal.h"

/**
 * AuraGameplayTags
 *
 * Singleton Containing native Gameplay Tags
 */

USTRUCT()
struct FMy_AuraGameplayTags
{
public:
	static const FMy_AuraGameplayTags& GetInstance()
	{
		return GameplayTagsInstance;
	}

	static  void InitializeNativeGameplayTags();
protected:


private:
	static  FMy_AuraGameplayTags GameplayTagsInstance;
};
