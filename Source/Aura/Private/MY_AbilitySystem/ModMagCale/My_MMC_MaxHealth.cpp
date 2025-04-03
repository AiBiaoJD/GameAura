// Copyright ABiao


#include "MY_AbilitySystem/ModMagCale/My_MMC_MaxHealth.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "My_Interraction/My_CombatInterface.h"

UMy_MMC_MaxHealth::UMy_MMC_MaxHealth()
{
	//初始化捕获属性
	VigorDef = FGameplayEffectAttributeCaptureDefinition(UMy_AuraAttributeSet::GetVIgorAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);

	//将捕获的属性添加到RelevantAttributesToCapture中
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMy_MMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Get Tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//属性聚合器的评估过程提供上下文信息
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float Vigor = 0.0f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max(Vigor, 0.0f);

	//CombatInterface是在Character_base中的,这样父类的指针可以调用不同子类的函数
	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 100.0f + 8.0f * (Vigor+30.0f) + 10.0f * PlayerLevel;
}





