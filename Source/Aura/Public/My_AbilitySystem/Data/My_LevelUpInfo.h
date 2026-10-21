// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "My_LevelUpInfo.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMy_LevelUpInfoDetail
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;
	
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointReward = 1;
	
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointReward = 1;
	
};

UCLASS()
class AURA_API UMy_LevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FMy_LevelUpInfoDetail> LevelUpInformation;

	int32 FindLevelForXP(int32 XP) const; 
};
