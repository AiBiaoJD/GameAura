// Copyright ABiao


#include "MY_AbilitySystem/ExeCalc/My_ExeCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"


struct My_AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	My_AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, BlockChance, Target, false);
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
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);

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

	// 获取格挡概率（BlockChance)
	float BlockChance = 0.f;
	const UMy_AuraAttributeSet* My_AuraAttribute = Cast<UMy_AuraAttributeSet>(TargetASC->GetAttributeSet(UMy_AuraAttributeSet::StaticClass()));
	if (My_AuraAttribute)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBlockChance (Direct) = %f"), My_AuraAttribute->GetBlockChance());
	}


	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluateParameters, BlockChance))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to capture BlockChance!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BlockChance = %f"), BlockChance);

	}
	const bool isBlocked = FMath::RandRange(0.f, 1.f) <= BlockChance;
	if (isBlocked)
	{
		Damage *= 0.5f;
	}


	// 输出伤害
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UMy_AuraAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage
	));
}
