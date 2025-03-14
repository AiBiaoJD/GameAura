// Copyright ABiao


#include "MY_AbilitySystem/Data/My_Attributeinfo.h"

FMy_AuraAttributeInfo UMy_Attributeinfo::FindAttributeinfoFormTag(const FGameplayTag& AttributeTag) const
{
	for (const FMy_AuraAttributeInfo& info : AttributeInformation)
	{
		if (info.AttributeTag == AttributeTag)
		{
			return  info;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Can not find info form AttributetTag [%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
	return FMy_AuraAttributeInfo();
}
