// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Aura/Public/My_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "My_AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraSummonAbility : public UMy_AuraGameplayAbilityBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "My_Ability|Summon")
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure, Category = "My_Ability|Summon")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Summon")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Summon")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Summon")
	float MinSpawnDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Summon")
	float MaxSpawnDistance= 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Summon")
	float SpawnSpread = 90.f;



	
};
