// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UMy_SpellMenuWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitiaValues() override;
	virtual void BindCallbacksToDependencies() override;
	
};
