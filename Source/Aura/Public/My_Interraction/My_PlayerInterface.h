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
	void AddToXP(int32 InXp);

	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();
};
