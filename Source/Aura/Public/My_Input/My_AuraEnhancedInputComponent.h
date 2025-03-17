// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "My_AuraInputConfig.h"
#include "My_AuraEnhancedInputComponent.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
	void BindAbilityAction(const UMy_AuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc);
};


template<class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
inline void UMy_AuraEnhancedInputComponent::BindAbilityAction(const UMy_AuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (auto& Temp : InputConfig->InputActionToTags)
	{
		if (Temp.InputAction && Temp.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Temp.InputAction, ETriggerEvent::Started, Object, PressedFunc, Temp.InputTag);
			}
			if (ReleaseFunc)
			{
				BindAction(Temp.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Temp.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Temp.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Temp.InputTag);
			}
		}
	}
}
