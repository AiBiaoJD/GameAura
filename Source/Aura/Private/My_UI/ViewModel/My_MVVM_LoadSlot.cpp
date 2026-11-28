// Copyright ABiao


#include "My_UI/ViewModel/My_MVVM_LoadSlot.h"

void UMy_MVVM_LoadSlot::InitializeSlot()
{
	/* enum class 不会隐式转 int，广播前要强转 */
	/* 蓝图收到这个 index 后切自己的 WidgetSwitcher：0=Vacant 1=EnterName 2=Taken */
	OnSetWidgetSwitcherIndex.Broadcast(static_cast<int32>(SlotStatus));
}

void UMy_MVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	/* 宏做三件事：比较是否变化 -> 赋值 -> 广播 FieldNotify */
	/* 必须走宏，直接写 PlayerName = X 不会通知界面 */
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMy_MVVM_LoadSlot::SetMapName(FString InMapName)
{
	/* 和 SetPlayerName 一样：走宏才会广播，直接赋值界面不刷新 */
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}
