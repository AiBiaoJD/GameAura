// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Data/My_Attributeinfo.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_AttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_OnAttributeInfoSignature, const FMy_AuraAttributeInfo&, info);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UMy_AttributeMenuWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()


public:

	//控制器广播数据给widget
	virtual void BroadcastInitiaValues();

	//ATTribute属性改变，委托调用的回调函数
	virtual void BindCallbacksToDependencies();

	UPROPERTY(BlueprintAssignable, Category = "My_GAS|Attibutes")
	FMy_OnAttributeInfoSignature OnAttributeInfo;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute DataAsset")
	TObjectPtr<UMy_Attributeinfo> AttributeDA;

private:
};
