// Copyright ABiao


#include "My_UI/HUD/My_LoadScreenHUD.h"

#include "My_UI/ViewModel/My_MVVM_LoadScreen.h"
#include "My_UI/Widget/My_LoadScreenWidget.h"

void AMy_LoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	/* 第一步：造界面级 VM，Outer 传 this 让它的生命周期跟着 HUD */
	LoadScreenViewModel = NewObject<UMy_MVVM_LoadScreen>(this, LoadScreenViewModelClass);
	/* 第二步：造出 3 个槽位 VM，此时它们还都是空的（状态 Vacant） */
	LoadScreenViewModel->InitializeLoadSlots();
	
	/* 第三步：造界面并上屏 */
	LoadScreenWidget = CreateWidget<UMy_LoadScreenWidget>(GetWorld(),LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	/* 敲门：让蓝图把 3 个槽位 VM 分发到 3 个槽位控件上，建立 View Binding */
	LoadScreenWidget->BluePrintInitializeWidget();

	/* 第四步：界面装配完成后才读盘 */
	/* 顺序不能反，LoadData 里 SetPlayerName 触发的广播需要有接收方，接收方是上一步建立的绑定 */
	LoadScreenViewModel->LoadData();
}
