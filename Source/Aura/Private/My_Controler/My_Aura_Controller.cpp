// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/Character.h"
#include "My_Input/My_AuraEnhancedInputComponent.h"
#include "My_Interraction/My_Enemy_Interface.h"
#include "My_UI/Widget/My_DamageTextComponent.h"

AMy_Aura_Controller::AMy_Aura_Controller()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AMy_Aura_Controller::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AMy_Aura_Controller::ClientShowDamageNum_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool IsBlockedHit, bool IsCriticalHit)
{
	UE_LOG(LogTemp, Warning, TEXT("4. Client RPC actually executing on client"));
	// 防御性检查：确保目标有效、组件类有效，且是本地客户端
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController()) 
	{
		UMy_DamageTextComponent* DamageText = NewObject<UMy_DamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //必须注册显示Widget
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, IsBlockedHit, IsCriticalHit);
	}
}


void AMy_Aura_Controller::AutoRun()
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn && bAutoRunning)
	{
		/*
		 * 当LMB的Release回调函数,正确设置Spline和bAutoRunning为真,启动自动寻路
		 */
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunningAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

//检查鼠标点击物体的一些函数
void AMy_Aura_Controller::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	// 保存上一帧的接口对象
	LastActor = ThisActor;
	// 将当前帧检测到的 Actor 转换为接口
	ThisActor = Cast<IMy_Enemy_Interface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AMy_Aura_Controller::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AMy_Aura_Controller::ShiftRelease);
	AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AMy_Aura_Controller::Move(const FInputActionValue& InputActionValue)
{
	bAutoRunning = false;
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightdDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

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
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// 左键点击敌人,激活相应Ability
	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagHeld(InputTag);
	}
	// 左键长按地面,进行移动F
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.Location;
		if (APawn* ControlledPawn = GetPawn())
		{
			FVector Direction = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(Direction);
		}
	}
}

void AMy_Aura_Controller::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// 不是左键点击
	if (!InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	// 左键松手不是点击敌人,也不是按Shift
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// 获取导航系统
			UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
			if (!NavSystem) return;

			// 查找路径
			if (UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				// 清除旧的 Spline 点
				Spline->ClearSplinePoints();

				// 遍历路径点并添加到 Spline
				for (const FVector& PathLoc : NavigationPath->PathPoints)
				{
					Spline->AddSplinePoint(PathLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PathLoc, 8.f, 8.f, FColor::Blue, false, 1.0f); // 绘制路径点
				}

				// 绘制路径线
				for (int32 i = 0; i < NavigationPath->PathPoints.Num() - 1; i++)
				{
					FVector StartPoint = NavigationPath->PathPoints[i];
					FVector EndPoint = NavigationPath->PathPoints[i + 1];
					DrawDebugLine(GetWorld(), StartPoint, EndPoint, FColor::Green, false, 1.0f, 0, 2.0f); // 绘制路径线
				}

				// 更新目标点为路径的最后一个点
				if (NavigationPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavigationPath->PathPoints[NavigationPath->PathPoints.Num() - 1];
				}
				bAutoRunning = true;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("No valid path found!"));
			}
		}
		FollowTime = 0.0f;
		bTargeting = false;
	}
	else
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagReleased(InputTag);
	}
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
