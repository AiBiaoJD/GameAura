// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "My_AuraInputConfig.generated.h"

USTRUCT(BlueprintType)
struct FMy_AuraInputActionToTag
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag = FGameplayTag();
};

/**
 *
 */
UCLASS()
class AURA_API UMy_AuraInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:

	const UInputAction* FindAbilityInputActionFromTag(const FGameplayTag& InputTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMy_AuraInputActionToTag> InputActionToTags;
};
