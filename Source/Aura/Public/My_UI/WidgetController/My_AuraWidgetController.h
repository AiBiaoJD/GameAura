// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UObject/NoExportTypes.h"
#include "My_AuraWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraWidgetController : public UObject
{
	GENERATED_BODY()
	/*
	 *widgetController��ȡ���ݴ�Mode�У��ٹ㲥��Widget
	 *������Ҫ�ǣ�ATTribute��ASC,PlayerState��PlayerController
	 */
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
};
