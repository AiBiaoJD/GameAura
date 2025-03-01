// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_UI/WidgetController/My_AuraWidgetController.h"
#include "My_OverlayWidgetController.generated.h"

//委托允许你将函数绑定到一个事件上，当事件发生时，所有绑定的函数都会被调用
//委托的核心思想是解耦：它允许你将事件的触发逻辑和事件的处理逻辑分开。例如，当一个角色的生命值发生变化时，你可以通过委托通知所有关心这个事件的系统，而不需要直接调用这些系统的函数。

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature, float, NewMaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnManaChangedSignature, float, NewMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxManaChangedSignature, float, NewMaxMana);


/**
 *
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UMy_OverlayWidgetController : public UMy_AuraWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitiaValues() override;

	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FOnManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attibutes")
	FOnMaxManaChangedSignature OnMaxManaChanged;

protected:
	void HealthChanged(const FOnAttributeChangeData& Date) const;
	void MaxHealthChanged(const FOnAttributeChangeData& Date) const ;
	void ManaChanged(const FOnAttributeChangeData& Data) const;
	void MaxManaChanged(const FOnAttributeChangeData& Data) const;



};

