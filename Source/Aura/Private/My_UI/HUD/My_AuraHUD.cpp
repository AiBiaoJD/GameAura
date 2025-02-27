// Copyright ABiao


#include "My_UI/HUD/My_AuraHUD.h"



UOverlayWidgetController* AMy_AuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WcParams)
{

	return nullptr;
}

void AMy_AuraHUD::BeginPlay()
{
	Super::BeginPlay();

	if (OverlayWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
		Widget->AddToViewport();
	}
}
