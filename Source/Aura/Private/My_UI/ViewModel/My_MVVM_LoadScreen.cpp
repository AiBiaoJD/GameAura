// Copyright ABiao


#include "My_UI/ViewModel/My_MVVM_LoadScreen.h"

#include "Kismet/GameplayStatics.h"
#include "My_GameMode/MyGameModeBase.h"
#include "My_GameMode/My_LoadScreenSaveGame.h"
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
	/* 点空槽位的"新建游戏"：状态切到 EnterName，InitializeSlot 会广播 1 */
	LoadSlots[Slot]->SlotStatus = EMy_SaveSlotStatus::EnterName;
	LoadSlots[Slot]->InitializeSlot();
}

void UMy_MVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(MyGameMode)) return;

	/* 第一步：把玩家输入写进 VM 的内存状态，界面绑定的名字会跟着刷新 */
	LoadSlots[Slot]->SetPlayerName(EnterName);

	/* 第二步：状态改成 Taken，下面 InitializeSlot 按它广播 2 */
	LoadSlots[Slot]->SlotStatus = EMy_SaveSlotStatus::Taken;

	/* 第三步：交给 GameMode 落盘，VM -> SaveGame -> 磁盘 */
	MyGameMode->SaveSlotData(LoadSlots[Slot]);

	/* 第四步：按刚设好的状态切页，不再硬编码 2 */
	LoadSlots[Slot]->InitializeSlot();
}

void UMy_MVVM_LoadScreen::TakenSlotButtonPressed(int32 Slot)
{
	if (!LoadSlots.Contains(Slot)) return;

	/* 先记住选中的是谁：广播读的是「此刻值」，顺序反了监听者拿到的是旧的 SelectedSlot */
	SelectedSlot = LoadSlots[Slot];
	OnSlotSelected.Broadcast();

	/* 遍历 3 个槽位：被点的那个禁用（它已被选中），其余恢复可点（可以改选） */
	/* 广播的是「能不能点」，不是「是不是选中」：false = 已选中，true = 可以改选我 */
	for (const TTuple<int32, UMy_MVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		LoadSlot.Value->OnEnableSelectSlotButton.Broadcast(LoadSlot.Key != Slot);
	}
}

void UMy_MVVM_LoadScreen::DeleteButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;

	/* 先删磁盘上的文件，再清 VM 状态 */
	AMyGameModeBase::DeleteSlot(SelectedSlot->SlotName, SelectedSlot->SlotIndex);

	/* 状态回 Vacant，InitializeSlot 按它切回空档页 */
	SelectedSlot->SlotStatus = EMy_SaveSlotStatus::Vacant;
	SelectedSlot->SetPlayerName(FString());
	SelectedSlot->InitializeSlot();

	/* 它现在又是空档了，按钮恢复可点 */
	SelectedSlot->OnEnableSelectSlotButton.Broadcast(true);

	/* 必须清掉选中：否则删完再点 Play，会拿一个空档槽位去跳地图 */
	SelectedSlot = nullptr;
}

void UMy_MVVM_LoadScreen::PlayButtonPressed()
{
	if (!IsValid(SelectedSlot)) return;

	AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(MyGameMode)) return;

	/* TODO: 把选中的槽位记进自己的 GameInstance，新关卡靠它读档 */

	MyGameMode->TravelToMap();
}

void UMy_MVVM_LoadScreen::LoadData()
{
	AMyGameModeBase* AuraGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode)) return;
	for (const TTuple<int32, UMy_MVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		UMy_LoadScreenSaveGame* LoadScreenSaveGame = AuraGameMode->GetSaveSlotData(LoadSlot.Value->SlotName, LoadSlot.Value->SlotIndex);
		if (!LoadScreenSaveGame) continue;
		
		LoadSlot.Value->SlotStatus = LoadScreenSaveGame->SlotStatus;
		LoadSlot.Value->SetPlayerName(LoadScreenSaveGame->PlayerName);
		LoadSlot.Value->InitializeSlot();
	}
}
