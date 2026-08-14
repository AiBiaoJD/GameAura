// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MY_AbilitySystem/Data/My_Attributeinfo.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_AttributeMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_OnAttributeInfoSignature, const FMy_AuraAttributeInfo&, info);
struct FGameplayTag;
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

	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UPROPERTY(BlueprintAssignable, Category = "My_GAS|Attributes")
	FMy_OnAttributeInfoSignature OnAttributeInfo;
	UPROPERTY(BlueprintAssignable, Category = "My_GAS|AttributePoint")
	FMy_OnPlayerStateChangedSignature OnPlayerAttributeChanged;
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attribute DataAsset")
	TObjectPtr<UMy_Attributeinfo> AttributeDA;

private:

	void BroadAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute);
};
