// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "My_AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	/*
	关联WidgetController
	widget -> widgetController ,
	控制器不需要知道那个widget关联他，只需要广播回去即可
	widget需要知道她的控制器是什么
	*/
public:
	/*
	 *这个函数必须在 C++ 中实现，蓝图只能调用它，不能修改它的实现。
	 *当你希望提供一个固定的功能接口WidgetController给蓝图使用,
	 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
protected:
	/*
	 这是一个蓝图实现的事件，目的是当我们设置WidgetController变量后，会在蓝图调用这个事件
	 可以在蓝图中更新 UI、绑定事件或执行其他初始化操作
	 TODO:理解这种解耦 Widget 和 WidgetController
	 Widget 需要知道它的 WidgetController，但 WidgetController 不需要知道具体的 Widget
	 通过 SetWidgetController 和 WidgetControllerSet，Widget 可以在 WidgetController 被设置后执行自定义逻辑，而不需要 WidgetController 直接操作 Widget。
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
