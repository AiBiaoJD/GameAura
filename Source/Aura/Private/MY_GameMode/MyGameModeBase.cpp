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
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName = LoadSlot->MapAssetName;
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

void AMyGameModeBase::TravelToMap(UMy_MVVM_LoadSlot* Slot) const
{
	if (!IsValid(Slot)) return;

	/* 槽位里记的是显示名，用它去 Maps 表里换出真正的关卡资产 */
	/* 教程这里用 FindChecked，key 不存在会直接断言崩溃；换成 Find 后查不到就走下面的兜底 */
	const TSoftObjectPtr<UWorld>* Map = Maps.Find(Slot->GetMapName());
	if (Map && !Map->IsNull())
	{
		/* 第一个参数是 WorldContext，传 this 即可 */
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, *Map);
		return;
	}

	/* 兜底：表里没这张图（比如 DefaultMapName 忘了配）就退回默认关卡，避免点 Play 没反应 */
	if (!DefaultMap.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, DefaultMap);
	}
}

FString AMyGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for (const TTuple<FString, TSoftObjectPtr<UWorld>>& Map : Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	/* 把默认地图登记进表里：DefaultMapName 就是它的显示名 */
	/* Maps 本身是 EditDefaultsOnly，蓝图里配的其它地图会在构造时就填好，这里只补默认图 */
	if (!DefaultMapName.IsEmpty() && !DefaultMap.IsNull())
	{
		Maps.Add(DefaultMapName, DefaultMap);
	}
}
