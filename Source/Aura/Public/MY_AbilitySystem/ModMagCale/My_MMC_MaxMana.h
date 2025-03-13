// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "My_MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_MMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMy_MMC_MaxMana();

	//核心函数:计算技能效果的数值
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

protected:
	//定义捕获的属性
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
	
};
