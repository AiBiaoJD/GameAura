// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "My_AuraWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FMY_WidgetControllerParams
{
	GENERATED_BODY()

	FMY_WidgetControllerParams()
	{
		
	}
	FMY_WidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS):PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS)
	{

	}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;

};


/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraWidgetController : public UObject
{
	GENERATED_BODY()
	/*
	 *widgetController获取数据从Mode中，再广播会Widget
	 *数据主要是：ATTribute，ASC,PlayerState，PlayerController
	 */
public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FMY_WidgetControllerParams& WCParams);

	//控制器广播数据给widget
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitiaValues();

	//ATTribute属性改变，委托调用的回调函数
	virtual void BindCallbacksToDependencies();
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};
