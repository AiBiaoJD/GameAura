// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "My_MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_SetWidgetSwitcherIndexSignature,int32, WidgetSwitcherIndex);
/**
 * 
 */
UCLASS()
class AURA_API UMy_MVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FMy_SetWidgetSwitcherIndexSignature OnSetWidgetSwitcherIndex;

	void InitializeSlot();
};
