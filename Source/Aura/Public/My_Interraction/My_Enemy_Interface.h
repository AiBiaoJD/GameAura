// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "My_Enemy_Interface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMy_Enemy_Interface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class AURA_API IMy_Enemy_Interface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;

};
