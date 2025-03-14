// Copyright ABiao


#include "My_UI/HUD/My_AuraHUD.h"



UMy_OverlayWidgetController* AMy_AuraHUD::GetOverlayWidgetController(const FMY_WidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UMy_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		//OverlayWidgetController创建好，就可以绑定回调函数(委托)来检查ATTribute变量改变
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return  OverlayWidgetController;
}

void AMy_AuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class unInit, fill out MY_BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class unInit, fill out MY_BP_AuraHUD"));

	// 设置OverlayWidget
	OverlayWidget = CreateWidget<UMy_AuraUserWidget>(GetWorld(), OverlayWidgetClass);

	// 设置OverlayWidgetController
	const FMY_WidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	OverlayWidgetController = GetOverlayWidgetController(WidgetControllerParams);

	// 关联OverlayWidget和OverlayWidgetController
	// 这样WidgetControllerSet()可以使用,Widget可以绑定函数给widgetController委托
	OverlayWidget->SetWidgetController(OverlayWidgetController);

	// 绑定完成后，widgetController就可以广播数据了
	OverlayWidgetController->BroadcastInitiaValues();

	// 将Widget添加到视口
	OverlayWidget->AddToViewport();
}
