// Copyright ABiao


#include "My_Character/Aura_Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "My_Controler/My_Aura_Controller.h"
#include "My_UI/HUD/My_AuraHUD.h"

AAura_Character::AAura_Character()
{

    // 角色在移动时自动朝向移动方向
    GetCharacterMovement()->bOrientRotationToMovement = true;
    // Yaw 左右转向
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 每秒旋转 540 度
    // 禁用平面约束（测试时可以先禁用）
    GetCharacterMovement()->bConstrainToPlane = false;
    GetCharacterMovement()->bSnapToPlaneAtStart = false;

    // 第三人称游戏设置
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AAura_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
    //服务器端初始化InitAbilityActorInfo
    My_InitAbilityActorInfo();
  
}

void AAura_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
    //客户端初始化InitAbilityActorInfo
    My_InitAbilityActorInfo();

}

void AAura_Character::My_InitAbilityActorInfo()
{
	//Init ability actor info for sever
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);

	//把PlayerState中的AbilitySystemComponent组件赋值给Aura_Character,避免野指针
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();


	//调用Initlayout(),在AMy_AuraHUD类中
	//因为这时候服务器和客户端的四个参数都为真
	//APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS
	//Todo:多人游戏客服端中，只有自己的Controller，其他人的controller为空，所以不用特判，跳过即可
	if (AMy_Aura_Controller* Aura_Controller = Cast<AMy_Aura_Controller>(GetController()))
	{
		if (AMy_AuraHUD* AuraHUD =Cast<AMy_AuraHUD>(Aura_Controller->GetHUD()))
		{
			AuraHUD->InitOverlay(Aura_Controller, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}


