// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "My_AuraSummonAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraSummonAbility : public UMy_AuraGameplayAbilityBase
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "MySummon")
	TArray<FVector> GetSpawnLocations();

	UFUNCTION(BlueprintPure, Category = "MySummon")
	TSubclassOf<APawn> GetRandomMinionClass();

	UPROPERTY(EditDefaultsOnly, Category = "MySummon")
	int32 NumMinions = 5;

	UPROPERTY(EditDefaultsOnly, Category = "MySummon")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "MySummon")
	float MinSpawnDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category = "MySummon")
	float MaxSpawnDistance= 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "MySummon")
	float SpawnSpread = 90.f;



	
};
