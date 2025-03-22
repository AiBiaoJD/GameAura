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
	if (bIsLocallyControlled)
	{
		SendMouseCursurData();
	}
	else
	{
		//TODO:On Server Listen Data
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

	// 服务器本地广播委托给蓝图
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
