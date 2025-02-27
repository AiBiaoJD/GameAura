// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "My_UI/Widget/My_AuraUserWidget.h"
#include "My_UI/WidgetController/My_OverlayWidgetController.h"
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


	//Ϊ�˳�ʼ��OverlayWidgetController��������Ҫ����FWidgetControllerParams�ṹ��
	//֮��ʹ��OverlayWidgetController����ʼ��OverlayWidget����Beginplay()�����Դ�����
	//�����½�һ������������������ʹ��BeginPLay
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_AuraUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UMy_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMy_OverlayWidgetController> OverlayWidgetControllerClass;
};
