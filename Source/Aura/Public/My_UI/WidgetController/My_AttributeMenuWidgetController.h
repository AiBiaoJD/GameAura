// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Data/My_Attributeinfo.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_AttributeMenuWidgetController.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMy_AttributeMenuWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()


public:

	//控制器广播数据给widget
	virtual void BroadcastInitiaValues();

	//ATTribute属性改变，委托调用的回调函数
	virtual void BindCallbacksToDependencies();


protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UMy_Attributeinfo> WidgetDataAsset;

private:
};
