// Copyright ABiao


#include "My_UI/HUD/My_AuraHUD.h"



void AMy_AuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class unInit, fill out MY_BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class unInit, fill out MY_BP_AuraHUD"));

	//设置OverlayWidget
	OverlayWidget = CreateWidget<UMy_AuraUserWidget>(GetWorld(), OverlayWidgetClass);

	//设置OverlayWidgetController
	FMY_WidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UMy_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
	}

	//关联OverlayWidget和OverlayWidgetController
	//这样就可以使用下面的函数在蓝图中了，当widget设置了他的widgetController就触发
	//UFUNCTION(BlueprintImplementableEvent)
	//void WidgetControllerSet();
	OverlayWidget->SetWidgetController(OverlayWidgetController);

	OverlayWidget->AddToViewport();

}

void AMy_AuraHUD::BeginPlay()
{
	Super::BeginPlay();


}
