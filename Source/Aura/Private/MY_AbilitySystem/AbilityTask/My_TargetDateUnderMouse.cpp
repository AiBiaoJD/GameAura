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
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	ValidData.Broadcast(CursorHit.Location);

}
