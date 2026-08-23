// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "MY_AbilitySystem/Data/My_AbilityInfo.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "My_Controler/My_Aura_Controller.h"
#include "My_AuraWidgetController.generated.h"


//更新PlayerState委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_OnPlayerStateChangedSignature, int32, NewValue);

//AbilityInfo委托,技能UI相关
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_AbilityInfoSignature, const FMy_AuraAbilityInfo&, AbilityInfo);

USTRUCT(BlueprintType)
struct FMY_WidgetControllerParams
{
	GENERATED_BODY()

	FMY_WidgetControllerParams()
	{
	}

	FMY_WidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS): PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS)
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

	UPROPERTY(BlueprintAssignable, Category = "My_GAS|AbilityUI")
	FMy_AbilityInfoSignature OnAbilityInfo;

	void BroadcastAbilityInfo();

protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<AMy_Aura_Controller> AuraPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AMy_AuraPlayerState> AuraPlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UMy_AuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UMy_AuraAttributeSet> AuraAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UMy_AbilityInfo> AbilityDA;

	AMy_Aura_Controller* GetAuraPC();
	AMy_AuraPlayerState* GetAuraPS();
	UMy_AuraAbilitySystemComponent* GetAuraASC();
	UMy_AuraAttributeSet* GetAuraAS();
};
