// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "My_AbilityActor/My_MagicCircle.h"
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
	UpdateMagicCircleLocation();
	AutoRun();
}

void AMy_Aura_Controller::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	/*
	 * 技能指示器是「操作者自己的 UI」，不是世界里的物体：
	 *   - 服务器没有真实鼠标，CursorHit 永远是零，生成出来只会钉在原点
	 *   - 别的客户端也不该看到，那是别人的准星
	 * 所以只在本地控制器上生成，配合 MagicCircle 的 bReplicates=false 保证不复制出去。
	 */
	if (!IsLocalController()) return;

	// 注意：光标显隐不在这里管，统一由 CursorTrace() 按"是否指着敌人"决定
	if (!IsValid(MagicCircle)) // 无效才创建
	{
		MagicCircle = GetWorld()->SpawnActor<AMy_MagicCircle>(MagicCircleClass);
	}

	if (IsValid(MagicCircle))
	{
		MagicCircle->SetMagicCircleMaterial(DecalMaterial);
	}
}

void AMy_Aura_Controller::HideMagicCircle()
{
	if (!IsLocalController()) return;

	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AMy_Aura_Controller::UpdateMagicCircleLocation()
{
	if (!IsLocalController()) return;

	/*
	 * 必须要求 bBlockingHit：鼠标指向天空时射线打不到东西，
	 * CursorHit.ImpactPoint 会是零向量，圆环直接被拽到世界原点 —— 看起来就是「位置固定不动」。
	 * 打空时保持上一帧位置，圆环停在原地，比瞬移合理。
	 */
	if (IsValid(MagicCircle) && CursorHit.bBlockingHit)
	{
		MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
	}
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
	// 本帧鼠标是否停在敌人身上（默认不是）
	bool bOverEnemy = false;

	/*
	 * 射线每帧都要更新，不能因为「禁止高亮」就整段跳过：
	 * MagicCircle 的位置完全靠 CursorHit 驱动，而施法期间玩家身上挂着
	 * Block_CursorTrace 标签 —— 一旦这里提前返回，圆环就钉在原地不动了。
	 * 所以「要不要高亮」和「要不要更新射线」是两件事，拆开处理。
	 */
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_CursorTrace))
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->UnHighlightActor();
		LastActor = nullptr;
		ThisActor = nullptr;
	}
	else
	{
		// 没打到东西就直接跳过：ThisActor 保持原样，高亮会残留（教程原行为）
		if (CursorHit.bBlockingHit)
		{
			// 保存上一帧的接口对象
			LastActor = ThisActor;
			// 将当前帧检测到的 Actor 转换为接口
			ThisActor = Cast<IMy_Enemy_Interface>(CursorHit.GetActor());
			bOverEnemy = (ThisActor != nullptr);

			if (LastActor != ThisActor)
			{
				if (LastActor) LastActor->UnHighlightActor();
				if (ThisActor) ThisActor->HighlightActor();
			}
		}
	}

	// 全函数唯一的出口：光标显隐统一在这里决定，不跟具体技能挂钩
	UpdateMouseCursor(bOverEnemy);
}

void AMy_Aura_Controller::UpdateMouseCursor(bool bOverEnemy)
{
	// bOverEnemy         世界现状：本帧鼠标是否在敌人身上
	// bHideCursorOnEnemy 我的策略：要不要隐藏
	// bCursorHiddenByMe  我的历史：上次的意图，决定这次该不该动手

	const bool bWantHide = bOverEnemy && bHideCursorOnEnemy;

	// 意图没变就不碰 bShowMouseCursor，把它让给别人（UI 等）管。
	// 注意：它可能和光标实际状态不一致，这是故意的——记的是意图，不是现状
	if (bCursorHiddenByMe == bWantHide) return;

	bCursorHiddenByMe = bWantHide;
	bShowMouseCursor = !bWantHide;
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
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_InputPressed))
	{
		return;
	}
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
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_InputPressed))
	{
		return;
	}
	if (InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false; // 废弃：改为LMB统一攻击
		bAutoRunning = false;
	}
	if (GetAuraASC()) GetAuraASC()->AbilityInputTagPressed(InputTag);
}


void AMy_Aura_Controller::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_InputHeld))
	{
		return;
	}

	// 不是左键点击
	if (!InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagHeld(InputTag);
		return;
	}

	// if (GetAuraASC()) GetAuraASC()->AbilityInputTagHeld(InputTag); // 【测试注释：恢复点击移动】

	// 左键点击敌人,激活相应Ability 【废弃：改为LMB统一攻击，不再区分敌人/地面】
	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagHeld(InputTag);
	}
	// 左键长按地面,进行移动
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
	if (GetAuraASC() && GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_InputReleased))
	{
		return;
	}
	// 不是左键点击
	if (!InputTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_InputTag_LMB))
	{
		if (GetAuraASC()) GetAuraASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	// 左键松手不是点击敌人,也不是按Shift — 点击移动寻路 【废弃：改为LMB统一攻击】
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
					//DrawDebugSphere(GetWorld(), PathLoc, 8.f, 8.f, FColor::Blue, false, 1.0f); // 绘制路径点
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
			if (GetAuraASC() && !GetAuraASC()->HasMatchingGameplayTag(FMy_AuraGameplayTags::GetInstance().My_Player_Block_InputPressed))
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickedNiagaraSystem, CachedDestination);
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
