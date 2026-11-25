// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "My_MVVM_LoadScreen.generated.h"

class UMy_MVVM_LoadSlot;
/**
 * 读档界面的界面级 ViewModel
 * 持有 3 个槽位 VM，负责跨槽位的操作（新建档、存盘、读盘）
 * 由 AMy_LoadScreenHUD 创建并初始化
 */
UCLASS()
class AURA_API UMy_MVVM_LoadScreen : public UMVVMViewModelBase 
{
	GENERATED_BODY()

public:
	/* 创建 3 个槽位 VM，设好各自的 SlotName 和 SlotIndex，塞进 LoadSlots 表 */
	void InitializeLoadSlots();

	/* 给蓝图用：按索引取槽位 VM，LoadScreen 分发 VM 给各个槽位控件时调用 */
	UFUNCTION(BlueprintPure)
	UMy_MVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index);

	/* 按钮入口：点空槽位的"新建游戏"，切到输入名字页 */
	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);
	
	/* 按钮入口：输入名字后点确认，写数据 + 存盘 + 切到已占用页 */
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot,const FString& EnterName);
	
	/* 按钮入口：点已有存档的槽位 */
	UFUNCTION(BlueprintCallable)
	void TakenSlotButtonPressed(int32 Slot);
	
	/* 从磁盘读 3 个槽位的数据灌进各自的 SlotVM，HUD 装配完界面后调用 */
	void LoadData();
protected:

	/* 槽位 VM 的类，蓝图里配成 My_MVVM_LoadSlot 的蓝图子类 */
	UPROPERTY(EditDefaultsOnly )
	TSubclassOf<UMy_MVVM_LoadSlot> LoadSlotViewModelClass;

private:

	/* 索引 -> 槽位 VM，遍历和按索引查找都靠这张表 */
	UPROPERTY()
	TMap<int32, UMy_MVVM_LoadSlot*> LoadSlots;

	/* 三个槽位的强引用，防止被 GC 回收（TMap 里已经引用了一次，这里是显式持有） */
	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_0;
	
	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_1;
	
	UPROPERTY()
	TObjectPtr<UMy_MVVM_LoadSlot> LoadSlot_2;

};
