// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Engine/DataAsset.h"
#include "My_CharacterClassInfo.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EMy_CharacterClass : uint8
{
	Elementalist, // 巫师
	Warrior,      // 战士
	Ranger		  // 远程
};

//每个敌人类，包含自己特别的能力等相关
USTRUCT(BlueprintType)
struct FMy_CharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};


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

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbility;

	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;

	FMy_CharacterClassDefaultInfo GetClassDefaultInfo(EMy_CharacterClass CharacterClass);
};
