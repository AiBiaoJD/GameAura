// Copyright ABiao


#include "My_Character/Aura_Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
