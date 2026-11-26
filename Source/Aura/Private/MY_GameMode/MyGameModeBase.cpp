// Copyright ABiao


#include "My_GameMode/MyGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "My_GameMode/My_LoadScreenSaveGame.h"
#include "My_UI/ViewModel/My_MVVM_LoadSlot.h"

void AMyGameModeBase::SaveSlotData(UMy_MVVM_LoadSlot* LoadSlot)
{
	if (!IsValid(LoadSlot) || LoadSlot->SlotName.IsEmpty()) return;

	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	UMy_LoadScreenSaveGame* LoadScreenSaveGame = Cast<UMy_LoadScreenSaveGame>(SaveGameObject);
	if (!LoadScreenSaveGame) return;

	LoadScreenSaveGame->SlotName = LoadSlot->SlotName;
	LoadScreenSaveGame->SlotIndex = LoadSlot->SlotIndex;
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotStatus = EMy_SaveSlotStatus::Taken;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->SlotName, LoadSlot->SlotIndex);
}

UMy_LoadScreenSaveGame* AMyGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	return Cast<UMy_LoadScreenSaveGame>(SaveGameObject);
}

void AMyGameModeBase::DeleteSlot(const FString& SlotName, int32 SlotIndex)
{
	/* DeleteGameInSlot 文件不存在时本来就返回 false，这里判断一下让意图更明确 */
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

void AMyGameModeBase::TravelToMap() const
{
	/* 没配地图就直接返回，避免打开一个空关卡 */
	if (DefaultMap.IsNull()) return;

	/* 第一个参数是 WorldContext，传 this 即可 */
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, DefaultMap);
}
