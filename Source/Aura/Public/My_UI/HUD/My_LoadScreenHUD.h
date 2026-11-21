// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "My_LoadScreenHUD.generated.h"

class UMy_MVVM_LoadScreen;
class UMy_LoadScreenWidget;
/**
 * 
 */
UCLASS()
class AURA_API AMy_LoadScreenHUD : public AHUD
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMy_LoadScreenWidget> LoadScreenWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMy_LoadScreenWidget> LoadScreenWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMy_MVVM_LoadScreen> LoadScreenViewModelClass;
	UPROPERTY(BlueprintReadOnly)
 	TObjectPtr<UMy_MVVM_LoadScreen> LoadScreenViewModel;
protected:
	virtual void BeginPlay() override;
};
