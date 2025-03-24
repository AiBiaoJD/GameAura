// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"
#include "My_UI/WidgetController/My_EnemyHealthWidgetController.h"
#include "My_UI/WidgetController/My_OverlayWidgetController.h"
#include "My_AuraAbilitySystemLibrary.generated.h"

class AEnemy_Characte;
/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_OverlayWidgetController* My_GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_AttributeMenuWidgetController* My_GetMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_EnemyHealthWidgetController* My_GetEnemyHealthWidgetController(const UObject* WorldContextObject, AEnemy_Characte* Enemy);
};
