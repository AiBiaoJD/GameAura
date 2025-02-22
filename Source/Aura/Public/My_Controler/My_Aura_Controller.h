// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
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
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
};