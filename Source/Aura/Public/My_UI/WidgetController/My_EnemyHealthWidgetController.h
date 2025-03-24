// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_OverlayWidgetController.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_EnemyHealthWidgetController.generated.h"


/**
 * 
 */
UCLASS()
class AURA_API UMy_EnemyHealthWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitiaValues();
	virtual void BindCallbacksToDependencies();


	UPROPERTY(BlueprintAssignable, Category = "My_GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnEnemyHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "My_GAS|Attibutes")
	FMy_OnAttributeChangedSignature OnEnemyMaxHealthChanged;
};
