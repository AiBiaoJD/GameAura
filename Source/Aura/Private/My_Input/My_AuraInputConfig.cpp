// Copyright ABiao


#include "My_Input/My_AuraInputConfig.h"

const UInputAction* UMy_AuraInputConfig::FindAbilityInputActionFromTag(const FGameplayTag& InputTag) const
{
	for (auto& Temp : InputActionToTags)
	{
		if (Temp.InputAction && Temp.InputTag == InputTag)
		{

			return Temp.InputAction;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("Can not find AbilityInputAction From InputTag [%s], On InputConfig [%s]"),*InputTag.ToString(), *GetNameSafe(this));
	return nullptr;
}
