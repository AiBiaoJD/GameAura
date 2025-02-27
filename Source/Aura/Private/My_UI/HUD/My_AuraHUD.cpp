// Copyright ABiao


#include "My_UI/HUD/My_AuraHUD.h"



void AMy_AuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class unInit, fill out MY_BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class unInit, fill out MY_BP_AuraHUD"));

	//����OverlayWidget
	OverlayWidget = CreateWidget<UMy_AuraUserWidget>(GetWorld(), OverlayWidgetClass);

	//����OverlayWidgetController
	FMY_WidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UMy_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WidgetControllerParams);
	}

	//����OverlayWidget��OverlayWidgetController
	//�����Ϳ���ʹ������ĺ�������ͼ���ˣ���widget����������widgetController�ʹ���
	//UFUNCTION(BlueprintImplementableEvent)
	//void WidgetControllerSet();
	OverlayWidget->SetWidgetController(OverlayWidgetController);

	OverlayWidget->AddToViewport();

}

void AMy_AuraHUD::BeginPlay()
{
	Super::BeginPlay();


}
