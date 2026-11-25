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
