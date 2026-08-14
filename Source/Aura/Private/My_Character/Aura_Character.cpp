// Copyright ABiao


#include "My_Character/Aura_Character.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/Data/My_LevelUpInfo.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "My_Controler/My_Aura_Controller.h"
#include "My_UI/HUD/My_AuraHUD.h"


AAura_Character::AAura_Character()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f)); // 俯 45° 斜向下
	SpringArmComponent->TargetArmLength = 800.f; // 弹簧臂长度
	SpringArmComponent->SetUsingAbsoluteRotation(true); // 相机上下视角独立，不跟随角色俯仰
	SpringArmComponent->bDoCollisionTest = false; // 关闭相机撞墙自动拉近功能，相机固定距离穿墙

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // 鼠标输入不驱动相机旋转，相机固定俯视，只转角色

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->bAutoActivate = false;

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

	CharacterClass = EMy_CharacterClass::Elementalist;
}

void AAura_Character::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//服务器端初始化InitAbilityActorInfo
	My_InitAbilityActorInfo();

	//初始化角色Ability
	AddCharacterAbilities();
}

void AAura_Character::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//客户端初始化InitAbilityActorInfo
	My_InitAbilityActorInfo();
}

int32 AAura_Character::FindLevelForXP_Implementation(int32 InXP)
{
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAura_Character::GetPlayerLevel_Implementation()
{
	const AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

int32 AAura_Character::GetXP_Implementation() const
{
	const AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetXP();
}

int32 AAura_Character::GetAttributePointReward_Implementation(int32 level) const
{
	const AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[level].AttributePointReward;
}

int32 AAura_Character::GetSpellPointReward_Implementation(int32 level) const
{
	const AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->LevelUpInfo->LevelUpInformation[level].SpellPointReward;
}

void AAura_Character::AddToXP_Implementation(int32 InXp)
{
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddXP(InXp);
}

void AAura_Character::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddToLevel(InPlayerLevel);
}

void AAura_Character::AddToAttributePoint_Implementation(int32 InAttributePoint)
{
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddAttributePoint(InAttributePoint);
}


void AAura_Character::AddToSpellPoint_Implementation(int32 InSpellPoint)
{
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddSpellPoint(InSpellPoint);
}

void AAura_Character::LevelUp_Implementation()
{
	MulticastLevelUpNiagaraFun();
}

void AAura_Character::MulticastLevelUpNiagaraFun_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCamera = (CameraLocation - NiagaraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCamera);
		LevelUpNiagaraComponent->Activate(true);
	}
}


void AAura_Character::My_InitAbilityActorInfo()
{
	//1.Init ability actor info for sever
	AMy_AuraPlayerState* AuraPlayerState = GetPlayerState<AMy_AuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);

	//2.当AbilityActorInfo设置后,就使用AbilityActorinfoSet(),进行代码耦合
	//处理捡东西的GE
	Cast<UMy_AuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	//把PlayerState中的AbilitySystemComponent组件赋值给Aura_Character,避免野指针
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();


	//3.调用Initlayout(),在AMy_AuraHUD类中
	//因为这时候服务器和客户端的四个参数都为真
	//APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS
	//Todo:多人游戏客服端中，只有自己的Controller，其他人的controller为空，所以不用特判，跳过即可
	if (AMy_Aura_Controller* Aura_Controller = Cast<AMy_Aura_Controller>(GetController()))
	{
		if (AMy_AuraHUD* AuraHUD = Cast<AMy_AuraHUD>(Aura_Controller->GetHUD()))
		{
			AuraHUD->InitOverlay(Aura_Controller, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	//4.使用Effect初始化Aura的PrimaryAttribute
	InitializeDefaultAttribute();
}
