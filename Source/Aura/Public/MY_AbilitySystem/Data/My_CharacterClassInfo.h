// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "My_CharacterClassInfo.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EMy_CharacterClass : uint8
{
	Elementalist, // 法师
	Warrior,      // 战士
	Ranger		  // 射手
};

USTRUCT(BlueprintType)
struct FMy_CharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};

/**
 *
 */
UCLASS()
class AURA_API UMy_CharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<EMy_CharacterClass, FMy_CharacterClassDefaultInfo> CharacterClassInformation;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	FMy_CharacterClassDefaultInfo GetClassDefaultInfo(EMy_CharacterClass CharacterClass);
};
