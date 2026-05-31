// Copyright ABiao


#include "MY_AbilitySystem/AbilityTask/My_TargetDateUnderMouse.h"
#include "My_Controler/My_Aura_Controller.h"

UMy_TargetDateUnderMouse* UMy_TargetDateUnderMouse::CreateMyTargetDateUnderMouse(UGameplayAbility* OwningAbility)
{
	UMy_TargetDateUnderMouse* MyObj = NewAbilityTask<UMy_TargetDateUnderMouse>(OwningAbility);
	return  MyObj;
}

void UMy_TargetDateUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();

	// 本地
	if (bIsLocallyControlled)
	{
		SendMouseCursurData();
	}
	// 服务器
	else
	{
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).AddUObject(this, &UMy_TargetDateUnderMouse::OnTargetDataReplicatedCallback);

		/*
		 * 竞态问题
		 * 服务器在上面委托绑定回调函数前,已经执行了委托的广播,错误的
		 * 因此要再广播上面的委托,直到服务器收到TargetDate
		 * ture表明收到TargetData;false为没收到TargetData,上面委托需要广播
		 */
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), GetActivationPredictionKey());
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}


}

void UMy_TargetDateUnderMouse::SendMouseCursurData()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit;
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// 客户端传TargetData给服务器
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	// 客户端(也可能是服务器)广播委托给蓝图
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UMy_TargetDateUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivateTag)
{
	// 服务器收到TargetData后,会自动存储TargetData,这里让服务器不需要再存储了
	AbilitySystemComponent.Get()->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}

}
