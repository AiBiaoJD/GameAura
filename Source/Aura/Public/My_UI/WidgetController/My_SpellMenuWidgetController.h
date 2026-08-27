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
	UPROPERTY(BlueprintAssignable, Category = "My_GAS|SpellPoints")
	FMy_OnPlayerStateChangedSignature OnPlayerSpellPointChanged;

	
	virtual void BroadcastInitiaValues() override;
	virtual void BindCallbacksToDependencies() override;
	
};
