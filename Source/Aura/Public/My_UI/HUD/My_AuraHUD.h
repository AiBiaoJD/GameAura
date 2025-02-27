// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "My_UI/Widget/My_AuraUserWidget.h"
#include "My_AuraHUD.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AMy_AuraHUD : public AHUD
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TObjectPtr<UMy_AuraUserWidget> OverlayWidget;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_AuraUserWidget> OverlayWidgetClass;
};
