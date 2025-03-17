// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "My_Input/My_AuraInputConfig.h"
#include "My_Interraction/My_Enemy_Interface.h"
#include "My_Aura_Controller.generated.h"


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
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);


	/*
	 * 鼠标高亮部分
	 */
	void CursorTrace();

	//TObjectPtr 只能用于 UObject 的派生类，不能用于接口，所以这里不用Tobject建立指针
	IMy_Enemy_Interface* LastEnemyInterface = nullptr;
	IMy_Enemy_Interface* ThisEnemyInterface = nullptr;

	/*
	 * AbilityInput部分
	 */
	UPROPERTY(EditAnywhere, Category = "My_Input")
	TObjectPtr<UMy_AuraInputConfig> InputConfig;

};