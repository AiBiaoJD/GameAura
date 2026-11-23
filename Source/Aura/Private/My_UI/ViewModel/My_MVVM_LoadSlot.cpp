// Copyright ABiao


#include "My_UI/ViewModel/My_MVVM_LoadSlot.h"

void UMy_MVVM_LoadSlot::InitializeSlot()
{
	// TODO:check Data
	OnSetWidgetSwitcherIndex.Broadcast(1);
}

void UMy_MVVM_LoadSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}
