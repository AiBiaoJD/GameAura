// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMy_SpellGlobeSelectSignature, bool, bSpendPointsButtonEnabled, bool, bEquipButtonEnabled);

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

	UPROPERTY(BlueprintAssignable)
	FMy_SpellGlobeSelectSignature OnSpellGlobeSelect;
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelecoted(const FGameplayTag& AbilityTag);

	virtual void BroadcastInitiaValues() override;
	virtual void BindCallbacksToDependencies() override;

private:
	static void My_ShouldEnableButtton(FGameplayTag AbilityStatus, const int32 SpellPoint, bool& bSpendPointsButtonEnabled, bool& bEquipButtonEnabled);
};
