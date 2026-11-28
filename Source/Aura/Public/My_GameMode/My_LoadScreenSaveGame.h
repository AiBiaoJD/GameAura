// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "My_LoadScreenSaveGame.generated.h"

/* 槽位状态：同时决定存档有没有内容、以及读档界面显示哪一页 */
/* 0=Vacant 空档  1=EnterName 正在输入名字  2=Taken 已有存档 */
UENUM(BlueprintType)
enum class EMy_SaveSlotStatus : uint8 { Vacant, EnterName, Taken };

/**
 * 读档界面的存档文件
 * 只存纯数据，本身没有逻辑，由 AMyGameModeBase 负责读写
 * 字段和 UMy_MVVM_LoadSlot 一一对应，两边靠 SaveSlotData / LoadData 搬运
 */
UCLASS()
class AURA_API UMy_LoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/* 槽位身份：存档文件名，决定磁盘上生成哪个 .sav */
	UPROPERTY()
	FString SlotName = FString();

	/* 槽位身份：存档索引，SaveGameToSlot 的第二个参数，PC 上不影响文件路径 */
	UPROPERTY()
	int32 SlotIndex = 0;

	/* 存档内容：玩家名，读档后显示在槽位上 */
	UPROPERTY()
	FString PlayerName = FString("Default Name");

	/* 存档内容：地图显示名，读档后显示在槽位上 */
	UPROPERTY()
	FString MapName = FString();

	/* 存档内容：地图资产名，和 MapName 一起记录玩家存档停在哪个关卡 */
	UPROPERTY()
	FString MapAssetName = FString();

	/* 存档内容：槽位状态，读档时写回 SlotVM 决定切哪一页 */
	UPROPERTY()
	EMy_SaveSlotStatus SlotStatus = EMy_SaveSlotStatus::Vacant;
};
