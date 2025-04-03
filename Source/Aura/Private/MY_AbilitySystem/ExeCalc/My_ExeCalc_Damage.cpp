// Copyright ABiao


#include "MY_AbilitySystem/ExeCalc/My_ExeCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"


struct My_AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	My_AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, BlockChance, Target, false);
	}
};
static const My_AuraDamageStatics& My_DamageStatics()
{
	static My_AuraDamageStatics My_DStatics;
	return My_DStatics;
}
UMy_ExeCalc_Damage::UMy_ExeCalc_Damage()
{
	RelevantAttributesToCapture.Add(My_DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().BlockChanceDef);

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

	// 获取Damage
	float Damage = Spec.GetSetByCallerMagnitude(FMy_AuraGameplayTags::GetInstance().My_EffectData_Damage);

	// 获取格挡概率(BlockChance)
	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);
	const bool isBlocked = FMath::RandRange(0.f, 1.f) <= TargetBlockChance;
	if (isBlocked)
	{
		Damage *= 0.5f;
	}

	// 获取护甲(Armor)
	float TargetArmor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	TargetArmor = FMath::Max(0.f, TargetArmor);

	// 获取护甲穿透(Armor)
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);

	// 根据护甲和护甲穿透计算Damage
	float EffectiveArmor = (1 - SourceArmorPenetration * 0.25) * TargetArmor;
	Damage *= (100 - EffectiveArmor * 0.25) / 100;

	// 输出伤害
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UMy_AuraAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage
	));
}
