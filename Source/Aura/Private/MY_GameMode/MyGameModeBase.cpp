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

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->SlotName, LoadSlot->SlotIndex);
}
