// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "My_Interraction/My_Enemy_Interface.h"

AMy_Aura_Controller::AMy_Aura_Controller()
{
	bReplicates = true;
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
	LastEnemyInterface = ThisEnemyInterface;
	// 将当前帧检测到的 Actor 转换为接口
	ThisEnemyInterface = Cast<IMy_Enemy_Interface>(CursorHit.GetActor());

	/*
	 *1. last = null, this = null, do nothing
	 *2. last = null  this = valid hightlight
	 *3. last = valid  this = null no hightlight
	 *4. last = valid  this = valid last!= this,no hi last ,  hi this
	 *5. last = valid  this = valid last == this,do nothing
	 */
	if (LastEnemyInterface == nullptr)
	{
		if (ThisEnemyInterface != nullptr)
		{
			//2
			ThisEnemyInterface->HighlightActor();
		}
		else
		{
			//1
		}
	}
	else
	{
		if (ThisEnemyInterface == nullptr)
		{
			//3
			LastEnemyInterface->UnHighlightActor();
		}
		else
		{
			//4
			if (LastEnemyInterface != ThisEnemyInterface)
			{
				LastEnemyInterface->UnHighlightActor();
				ThisEnemyInterface->UnHighlightActor();
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
	check(Subsystem);

	//子系统关联输入上下文
	Subsystem->AddMappingContext(AuraContext, 0);

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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	//处理input Action,将设备输入绑定到input Action,在设备输入调用函数Move
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMy_Aura_Controller::Move);
}

void AMy_Aura_Controller::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	//UE_LOG(LogTemp, Warning, TEXT("InputAxisVector: X=%f, Y=%f"), InputAxisVector.X, InputAxisVector.Y);

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

