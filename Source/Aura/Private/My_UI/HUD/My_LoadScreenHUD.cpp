// Copyright ABiao


#include "My_UI/HUD/My_LoadScreenHUD.h"

#include "My_UI/ViewModel/My_MVVM_LoadScreen.h"
#include "My_UI/Widget/My_LoadScreenWidget.h"

void AMy_LoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel = NewObject<UMy_MVVM_LoadScreen>(this, LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlots();

	
	LoadScreenWidget = CreateWidget<UMy_LoadScreenWidget>(GetWorld(),LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BluePrintInitializeWidget();
}
