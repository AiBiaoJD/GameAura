// Copyright ABiao


#include "My_UI/HUD/My_AuraHUD.h"

void AMy_AuraHUD::BeginPlay()
{
	Super::BeginPlay();

	if (OverlayWidgetClass)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
		Widget->AddToViewport();
	}
}
