// Copyright ABiao


#include "MY_AbilitySystem/ExeCalc/My_ExeCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"


struct My_AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	My_AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, Armor, Target, false);
	}
};
static const My_AuraDamageStatics& DamageStatics()
{
	static My_AuraDamageStatics DStatics;
	return DStatics;
}
UMy_ExeCalc_Damage::UMy_ExeCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
}


void UMy_ExeCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 获取捕获的属性值
	float Armor;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluateParameters, Armor);
	Armor = FMath::Max(0.f, Armor);
	Armor++;

	// 输出伤害
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		DamageStatics().ArmorProperty,
		EGameplayModOp::Additive,
		Armor
	));
}
