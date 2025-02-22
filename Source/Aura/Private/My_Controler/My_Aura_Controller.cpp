// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AMy_Aura_Controller::AMy_Aura_Controller()
{
	bReplicates = true;
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
