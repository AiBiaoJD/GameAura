// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "My_PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMy_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IMy_PlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 InXP);
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetXP() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointReward(int32 level) const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePointFormPlayerState() const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointReward(int32 level) const;
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPointFormPlayerState() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void AddToXP(int32 InXp);
	UFUNCTION(BlueprintNativeEvent)
	void AddToPlayerLevel(int32 InPlayerLevel);
	UFUNCTION(BlueprintNativeEvent)
	void AddToAttributePoint(int32 InAttributePoint);
	UFUNCTION(BlueprintNativeEvent)
	void AddToSpellPoint(int32 InSpellPoint);
	
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
