// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "My_Input/My_AuraEnhancedInputComponent.h"
#include "My_Interraction/My_Enemy_Interface.h"

AMy_Aura_Controller::AMy_Aura_Controller()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

}

void AMy_Aura_Controller::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();

}

//检查鼠标点击物体的一些函数
void AMy_Aura_Controller::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// 保存上一帧的接口对象
	LastActor = ThisActor;
	// 将当前帧检测到的 Actor 转换为接口
	ThisActor = Cast<IMy_Enemy_Interface>(CursorHit.GetActor());

	/*
	 *1. last = null, this = null, do nothing
	 *2. last = null  this = valid hightlight
	 *3. last = valid  this = null no hightlight
	 *4. last = valid  this = valid last!= this,no hi last ,  hi this
	 *5. last = valid  this = valid last == this,do nothing
	 */
	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			//2
			ThisActor->HighlightActor();
		}
		else
		{
			//1
		}
	}
	else
	{
		if (ThisActor == nullptr)
		{
			//3
			LastActor->UnHighlightActor();
		}
		else
		{
			//4
			if (LastActor != ThisActor)
			{
				LastActor->UnHighlightActor();
				ThisActor->UnHighlightActor();
			}
			else
			{
				//5
			}
		}
	}

}


void AMy_Aura_Controller::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	//子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//其他客户端的subsystem是空，不能用check，因为是合理的，直接跳过
	if (Subsystem)
	{
		//子系统关联输入上下文
		Subsystem->AddMappingContext(AuraContext, 0);
	}



	//鼠标设置
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//用于设置游戏和 UI 的输入模式
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode((InputModeData));


}

void AMy_Aura_Controller::SetupInputComponent()
{
	Super::SetupInputComponent();

	UMy_AuraEnhancedInputComponent* AuraInputComponent = CastChecked<UMy_AuraEnhancedInputComponent>(InputComponent);

	//处理input Action,将设备输入绑定到input Action,在设备输入调用函数Move
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMy_Aura_Controller::Move);

	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AMy_Aura_Controller::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();


	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector  ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector  RightdDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn())
	{
		//这里InputAxisVector.Y向前，是因为我们定义action中y为前进
		//而世界中x是前进,所有dir那里用了X
		ControlledPawn->AddMovementInput(ForwardDir, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightdDir, InputAxisVector.X);
	}

}


void AMy_Aura_Controller::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}

	
}


void AMy_Aura_Controller::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 不是左键点击
	if (!InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		if (GetAuraASC() == nullptr) return;
		GetAuraASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// 左键点击敌人,激活相应Ability
	if (bTargeting)
	{
		if (GetAuraASC() == nullptr) return;
		GetAuraASC()->AbilityInputTagHeld(InputTag);
	}
	// 左键点击地面,进行移动
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		FHitResult Hit;
		if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		{
			CachedDestination = Hit.Location;

		}
		if (APawn* ControlledPawn = GetPawn())
		{
			FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(Direction);
		}
	}

}

void AMy_Aura_Controller::AbilityInputTagReleased(FGameplayTag InputTag)
{

	if (GetAuraASC() == nullptr) return;
	GetAuraASC()->AbilityInputTagReleased(InputTag);

	
}


// 确保只cast一次,避免在InputAction回调函数,每帧调用
UMy_AuraAbilitySystemComponent* AMy_Aura_Controller::GetAuraASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UMy_AuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}
