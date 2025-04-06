// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Data/My_CharacterClassInfo.h"
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
	static UMy_OverlayWidgetController* My_GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static UMy_AttributeMenuWidgetController* My_GetMenuWidgetController(const UObject* WorldContextObject);

	/*
	 * Enemy
	 */
	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, EMy_CharacterClass CharacterType, float level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static void InitStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static UMy_CharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	/*
	 * Effect Context
	 */
	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool IsBlocked);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool IsCritical);
};
