// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "My_LoadScreenWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_LoadScreenWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, blueprintCallable)
	void BluePrintInitializeWidget();
}; 
