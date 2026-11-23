// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "My_MVVM_LoadSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_SetWidgetSwitcherIndexSignature,int32, WidgetSwitcherIndex);
/**
 * 
 */
UCLASS()
class AURA_API UMy_MVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:

	/* 切页委托：由 ScreenVM 广播，槽位收到后切自己的 WidgetSwitcher */
	UPROPERTY(BlueprintAssignable)
	FMy_SetWidgetSwitcherIndexSignature OnSetWidgetSwitcherIndex;

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

private:

	/* 存档内容：玩家名 */
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess="true"))
	FString PlayerName;
};
