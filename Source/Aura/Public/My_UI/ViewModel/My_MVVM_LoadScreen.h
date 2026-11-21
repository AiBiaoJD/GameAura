// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "My_MVVM_LoadScreen.generated.h"

class UMy_MVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMy_MVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlots();

	UFUNCTION(BlueprintPure)
	UMy_MVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index);

protected:

	UPROPERTY(EditDefaultsOnly )
	TSubclassOf<UMy_MVVM_LoadSlot> LoadSlotViewModelClass;

private:

	UPROPERTY()
	TMap<int32, UMy_MVVM_LoadSlot*> LoadSlots;

	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_0;
	
	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_1;
	
	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_2;

};
