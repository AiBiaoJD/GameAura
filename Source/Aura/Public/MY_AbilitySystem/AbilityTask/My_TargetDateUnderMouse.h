// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "My_TargetDateUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_OnMouseTargetDateSignature, const FGameplayAbilityTargetDataHandle&, TargetDataHandle);
/**
 * 
 */
UCLASS()
class AURA_API UMy_TargetDateUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	/*
	 * 仿照UAbilityTask_PlayMontageAndWait,实现自己的Task
	 * Task要用函数返回自己的实例Instance
	 */
	UFUNCTION(BlueprintCallable, Category="My_Ability|Tasks", meta=(DisplayName = "MyTargetDateUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UMy_TargetDateUnderMouse* CreateMyTargetDateUnderMouse(UGameplayAbility* OwningAbility);

	/*
	 * 委托作为蓝图中上面函数的输出引脚
	 */
	UPROPERTY(BlueprintAssignable)
	FMy_OnMouseTargetDateSignature ValidData;

private:
	// Task创建ASC自动调用下面的函数
	virtual void Activate() override;
	void SendMouseCursurData();
};
