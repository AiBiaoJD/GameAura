// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "My_UI/Widget/My_AuraUserWidget.h"
#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"
#include "My_UI/WidgetController/My_OverlayWidgetController.h"
#include "My_UI/WidgetController/My_SpellMenuWidgetController.h"
#include "My_AuraHUD.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AMy_AuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UMy_OverlayWidgetController* GetOverlayWidgetController(const FMY_WidgetControllerParams& WCParams);
	UMy_AttributeMenuWidgetController* GetAttributeMenuWidgetController(const FMY_WidgetControllerParams& WCParams);
	UMy_SpellMenuWidgetController* GetSpellMenuWidgetController(const FMY_WidgetControllerParams& WCParams);

	//为了初始化OverlayWidgetController，我们需要传入FWidgetControllerParams结构体
	//之后使用OverlayWidgetController来初始化OverlayWidget，而Beginplay()不可以传参数
	//所以新建一个函数来传参数，不使用BeginPLay
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:

private:
	UPROPERTY()
	TObjectPtr<UMy_AuraUserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_AuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UMy_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_OverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UMy_AttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_AttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UMy_SpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_SpellMenuWidgetController> SpellMenuWidgetControllerClass;
};
