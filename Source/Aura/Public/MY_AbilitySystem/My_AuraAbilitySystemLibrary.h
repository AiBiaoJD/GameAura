// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"
#include "My_UI/WidgetController/My_OverlayWidgetController.h"
#include "My_AuraAbilitySystemLibrary.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_OverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_AttributeMenuWidgetController* GetMenuWidgetController(const UObject* WorldContextObject);
};
