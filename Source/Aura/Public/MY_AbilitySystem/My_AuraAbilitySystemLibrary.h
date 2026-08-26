// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Data/My_CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "My_AuraAbilitySystemLibrary.generated.h"

class UMy_AbilityInfo;
class UMy_SpellMenuWidgetController;
class UMy_OverlayWidgetController;
class UMy_AttributeMenuWidgetController;

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController")
	static bool MakeWidgetControllerParams(const UObject* WorldContextObject, FMY_WidgetControllerParams& Params, AMy_AuraHUD*& HUD);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UMy_OverlayWidgetController* My_GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UMy_AttributeMenuWidgetController* My_GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_WidgetController", meta=(DefaultToSelf="WorldContextObject"))
	static UMy_SpellMenuWidgetController* My_GetSpellMenuWidgetController(const UObject* WorldContextObject);
	/*
	 * Enemy
	 */
	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static void GiveDefaultAttributes(const UObject* WorldContextObject, EMy_CharacterClass CharacterType, float level, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, EMy_CharacterClass CharacterType);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_CharacterClassDefault")
	static UMy_CharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_AbilityInfoDefault")
	static UMy_AbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

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


	// Attack Part: Get Actor from sphere
	UFUNCTION(BlueprintCallable, Category = "My_AuraAbilitySystemLibrary|My_GameplayMachanics")
	static void GetLivePlayersWithRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverLappingActors, const TArray<AActor*>& ActorToIgnore, float Radius, const FVector& SphereOrigin);

	UFUNCTION(BlueprintPure, Category = "My_AuraAbilitySystemLibrary|My_GameplayMachanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);

	// XP Reward
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, EMy_CharacterClass CharacterType, int32 level);
};
