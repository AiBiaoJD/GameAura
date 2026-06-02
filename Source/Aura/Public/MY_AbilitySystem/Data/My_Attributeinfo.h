// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "My_Attributeinfo.generated.h"


//Attribute改变时，Controller传给Widget的结构
USTRUCT(BlueprintType)
struct FMy_AuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();
	
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.0f;
};

/**
 * 
 */
UCLASS()
class AURA_API UMy_Attributeinfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FMy_AuraAttributeInfo FindAttributeinfoFormTag(const FGameplayTag& AttributeTag) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FMy_AuraAttributeInfo> AttributeInformation;

	
};
