// Copyright ABiao


#include "My_UI/ViewModel/My_MVVM_LoadScreen.h"

#include "Kismet/GameplayStatics.h"
#include "My_GameMode/MyGameModeBase.h"
#include "My_UI/ViewModel/My_MVVM_LoadSlot.h"

void UMy_MVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMy_MVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SlotName = FString("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);

	LoadSlot_1 = NewObject<UMy_MVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SlotName = FString("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);

	LoadSlot_2 = NewObject<UMy_MVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SlotName = FString("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);
}

UMy_MVVM_LoadSlot* UMy_MVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index)
{
	return LoadSlots.FindChecked(Index);
}

void UMy_MVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->OnSetWidgetSwitcherIndex.Broadcast(1);
}

void UMy_MVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(MyGameMode)) return;

	LoadSlots[Slot]->SetPlayerName(EnterName);

	MyGameMode->SaveSlotData(LoadSlots[Slot]);

	LoadSlots[Slot]->OnSetWidgetSwitcherIndex.Broadcast(2);
}

void UMy_MVVM_LoadScreen::TakenSlotButtonPressed(int32 Slot)
{
}
