// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "My_DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_DamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage, bool IsBlockedHit, bool IsCriticalHit);
};
