// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "Components/SplineComponent.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "My_Input/My_AuraInputConfig.h"
#include "My_Aura_Controller.generated.h"


class UDamageTextComponent;
class UMy_DamageTextComponent;
class IMy_Enemy_Interface;
/**
 *
 */
UCLASS()
class AURA_API AMy_Aura_Controller : public APlayerController
{
	GENERATED_BODY()
public:
	AMy_Aura_Controller();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ClientShowDamageNum(float DamageAmount, ACharacter* TargetCharacter,bool IsBlockedHit, bool IsCriticalHit);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UInputAction> ShiftAction;

	bool bShiftKeyDown = false;


	void Move(const FInputActionValue& InputActionValue);
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftRelease() { bShiftKeyDown = false; };
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);


	/*
	 * 鼠标高亮部分
	 */
	void CursorTrace();
	FHitResult CursorHit;
	//TObjectPtr 只能用于 UObject 的派生类，不能用于接口，所以这里不用Tobject建立指针
	IMy_Enemy_Interface* LastActor = nullptr;
	IMy_Enemy_Interface* ThisActor = nullptr;

	/*
	 * AbilityInput
	 */
	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UMy_AuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UMy_AuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UMy_AuraAbilitySystemComponent* GetAuraASC();


	/*
	 * ClickToMove
	 */

	FVector CachedDestination = FVector::Zero();
	float FollowTime = 0.f;
	bool bAutoRunning = false;
	bool bTargeting = false;					//鼠标是否点击敌人，用来处理鼠标左键多种Ability
	float ShortPressThreshold = 0.5f;			//短按的阀值

	UPROPERTY(EditDefaultsOnly)
	float AutoRunningAcceptanceRadius = 50.0f; //自动寻路停止距离
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();

	/* Damage Text Show */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMy_DamageTextComponent> DamageTextComponentClass;


};

