// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "My_GameMode/My_LoadScreenSaveGame.h"
#include "My_MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_SetWidgetSwitcherIndexSignature,int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_EnableSelectSlotButtonSignature, bool, bEnable);
/**
 * 单个存档槽位的 ViewModel
 * 一个槽位一个实例，由 UMy_MVVM_LoadScreen::InitializeLoadSlots 创建
 * 上半部分是槽位身份和内部数据，private 里是真正驱动 UI 的显示字段
 */
UCLASS()
class AURA_API UMy_MVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	/* 切页委托：由 ScreenVM 广播，槽位收到后切自己的 WidgetSwitcher */
	UPROPERTY(BlueprintAssignable)
	FMy_SetWidgetSwitcherIndexSignature OnSetWidgetSwitcherIndex;
	
	/* 按钮可用性委托：false = 已被选中（再点没意义），true = 可以改选我 */
	UPROPERTY(BlueprintAssignable)
	FMy_EnableSelectSlotButtonSignature OnEnableSelectSlotButton;
	
	/* 按当前存档状态广播切页，0=Vacant 1=EnterName 2=Taken */
	void InitializeSlot();

	/* 槽位身份：存档文件名，SaveGameToSlot 的第一个参数 */
	UPROPERTY()
	FString SlotName;

	/* 槽位身份：存档索引，SaveGameToSlot 的第二个参数，PC 上不影响文件路径 */
	UPROPERTY()
	int32 SlotIndex;

	/* 玩家名：写入时走 Setter，值变化会广播 FieldNotify 驱动 UI */
	void SetPlayerName(FString InPlayerName);
	FString GetPlayerName() const { return PlayerName; }

	/* 槽位状态：Vacant/EnterName/Taken，InitializeSlot 据此决定广播哪一页 */
	/* 和 SaveGame 里的同名字段对应，存盘时写出去，读档时读回来 */
	UPROPERTY()
	EMy_SaveSlotStatus SlotStatus = EMy_SaveSlotStatus::Vacant;

private:

	/* 存档内容：玩家名，界面不直接显示，靠 FieldNotify 通知绑定的 TextBlock */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString PlayerName;
};
