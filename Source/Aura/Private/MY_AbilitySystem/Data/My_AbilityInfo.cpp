// Copyright ABiao


#include "MY_AbilitySystem/Data/My_AbilityInfo.h"

#include "Aura/AuraLogChannels.h"

FMy_AuraAbilityInfo UMy_AbilityInfo::FindAbilityInfoFromTag(const FGameplayTag& AbilityTag) const
{
	for (const FMy_AuraAbilityInfo& info :AbilityInformation)
	{
		if (info.AbilityTag == AbilityTag)
		{
			return info;
		}
	}
	UE_LOG(LogAura, Error, TEXT("Can not find info form AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	return FMy_AuraAbilityInfo();
}
