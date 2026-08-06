// Copyright ABiao


#include "MY_AbilitySystem/ModMagCale/My_MMC_MaxMana.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "My_Interraction/My_CombatInterface.h"

UMy_MMC_MaxMana::UMy_MMC_MaxMana()
{
	//初始化捕获属性
	IntelligenceDef = FGameplayEffectAttributeCaptureDefinition(UMy_AuraAttributeSet::GetIntelligenceAttribute(), EGameplayEffectAttributeCaptureSource::Source, false
	);


	//将捕获的属性添加到RelevantAttributesToCapture中
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMy_MMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTag = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTag = Spec.CapturedSourceTags.GetAggregatedTags();


	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTag;
	EvaluateParameters.TargetTags = TargetTag;

	float Intelligence = 0.0f;
	GetCapturedAttributeMagnitude(IntelligenceDef, Spec, EvaluateParameters, Intelligence);
	Intelligence = FMath::Max(0.0f, Intelligence);

	int32 Level = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UMy_CombatInterface>())
	{
		Level = IMy_CombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}


	return 100.0f + 8.0f * (Intelligence + 30.0f) + 10.0f * Level;
}
