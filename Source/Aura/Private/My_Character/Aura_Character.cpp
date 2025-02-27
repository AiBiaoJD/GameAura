// Copyright ABiao


#include "My_Character/Aura_Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "My_Controler/My_Aura_Controller.h"
#include "My_UI/HUD/My_AuraHUD.h"

AAura_Character::AAura_Character()
{

    // ��ɫ���ƶ�ʱ�Զ������ƶ�����
    GetCharacterMovement()->bOrientRotationToMovement = true;
    // Yaw ����ת��
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ÿ����ת 540 ��
    // ����ƽ��Լ��������ʱ�����Ƚ��ã�
    GetCharacterMovement()->bConstrainToPlane = false;
    GetCharacterMovement()->bSnapToPlaneAtStart = false;

    // �����˳���Ϸ����
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;
}

void AAura_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
    //�������˳�ʼ��InitAbilityActorInfo
    My_InitAbilityActorInfo();
  
}

void AAura_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
    //�ͻ��˳�ʼ��InitAbilityActorInfo
    My_InitAbilityActorInfo();

}

void AAura_Character::My_InitAbilityActorInfo()
{
	//Init ability actor info for sever
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);

	//��PlayerState�е�AbilitySystemComponent�����ֵ��Aura_Character,����Ұָ��
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();


	//����Initlayout(),��AMy_AuraHUD����
	//��Ϊ��ʱ��������Ϳͻ��˵��ĸ�������Ϊ��
	//APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS
	//Todo:������Ϸ�ͷ����У�ֻ���Լ���Controller�������˵�controllerΪ�գ����Բ������У���������
	if (AMy_Aura_Controller* Aura_Controller = Cast<AMy_Aura_Controller>(GetController()))
	{
		if (AMy_AuraHUD* AuraHUD =Cast<AMy_AuraHUD>(Aura_Controller->GetHUD()))
		{
			AuraHUD->InitOverlay(Aura_Controller, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}


