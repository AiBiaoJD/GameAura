// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "My_AbilityInfo.generated.h"

/**
 * 
 */
class UGameplayAbility;
USTRUCT(BlueprintType)
struct FMy_AuraAbilityInfo
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag StatusTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag CoolDownTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> Background = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 LevelUpRequirement = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityClass = nullptr;
};


UCLASS()
class AURA_API UMy_AbilityInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	FMy_AuraAbilityInfo FindAbilityInfoFromTag(const FGameplayTag& AbilityTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMy_AuraAbilityInfo> AbilityInformation;
};
