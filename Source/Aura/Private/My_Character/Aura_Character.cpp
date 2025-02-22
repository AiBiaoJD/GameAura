// Copyright ABiao


#include "My_Character/Aura_Character.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
