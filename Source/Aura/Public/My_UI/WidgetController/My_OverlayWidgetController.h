// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_UI/Widget/My_AuraUserWidget.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_OverlayWidgetController.generated.h"

//这个结构主要信息显示到屏幕
USTRUCT(BlueprintType)
struct FMy_UIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UMy_AuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};


//委托允许你将函数绑定到一个事件上，当事件发生时，所有绑定的函数都会被调用
//委托的核心思想是解耦：它允许你将事件的触发逻辑和事件的处理逻辑分开。例如，当一个角色的生命值发生变化时，你可以通过委托通知所有关心这个事件的系统，而不需要直接调用这些系统的函数。

//更新Attribute委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_OnAttributeChangedSignature, float, NewValue);

//Message委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_MessageWidgetRowSignature, FMy_UIWidgetRow, Row);

/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UMy_OverlayWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitiaValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Message")
	FMy_MessageWidgetRowSignature OnMessageWidgetRow;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	//获取数据表的特定行
	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);



};

template<typename T>
inline T* UMy_OverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}
