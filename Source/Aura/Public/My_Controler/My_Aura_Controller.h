// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
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
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();

	IMy_Enemy_Interface* LastEnemyInterface = nullptr;
	IMy_Enemy_Interface* ThisEnemyInterface = nullptr;

};