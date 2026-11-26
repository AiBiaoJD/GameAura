// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

class UMy_LoadScreenSaveGame;
class UMy_MVVM_LoadSlot;
class UMy_AbilityInfo;
class UMy_CharacterClassInfo;
class UWorld;
/**
 * 主菜单和游戏内的 GameMode
 * 存档的读写都放在这里：VM 只管内存状态，真正碰磁盘的是这一层
 */
UCLASS()
class AURA_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="Character Class Defaults")
	TObjectPtr<UMy_CharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly,Category="Ability Info Defaults")
	TObjectPtr<UMy_AbilityInfo> AbilityInfo;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMy_LoadScreenSaveGame> LoadScreenSaveGameClass;

	/* 存盘：从 SlotVM 取数据填进 SaveGame 对象，再写磁盘 */
	void SaveSlotData(UMy_MVVM_LoadSlot* LoadSlot);

	/* 读档：有文件就读出来，没有就返回一个空的 SaveGame 对象（状态 Vacant） */
	UMy_LoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const ;

	/* 删除某个槽位的存档文件，文件不存在时静默返回 */
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);

	/* 新建存档 / 点 Play 时进入的主关卡 */
	UPROPERTY(EditDefaultsOnly, Category="Map Defaults")
	TSoftObjectPtr<UWorld> DefaultMap;

	/* 跳到 DefaultMap。关卡切换是异步的，这个 GameMode 在之后会被销毁 */
	void TravelToMap() const;

};
