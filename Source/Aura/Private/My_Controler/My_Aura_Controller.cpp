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

//�������������һЩ����
void AMy_Aura_Controller::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// ������һ֡�Ľӿڶ���
	LastEnemyInterface = ThisEnemyInterface;
	// ����ǰ֡��⵽�� Actor ת��Ϊ�ӿ�
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

	//��ϵͳ
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	//��ϵͳ��������������
	Subsystem->AddMappingContext(AuraContext, 0);

	//�������
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	//����������Ϸ�� UI ������ģʽ
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode((InputModeData));


}

void AMy_Aura_Controller::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	//����input Action,���豸����󶨵�input Action,���豸������ú���Move
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
		//����InputAxisVector.Y��ǰ������Ϊ���Ƕ���action��yΪǰ��
		//��������x��ǰ��,����dir��������X
		ControlledPawn->AddMovementInput(ForwardDir, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightdDir, InputAxisVector.X);
	}

}

