// Copyright ABiao


#include "MY_AbilitySystem/ExeCalc/My_ExeCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include <MY_AbilitySystem/Data/My_CharacterClassInfo.h>

#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "My_Interraction/My_CombatInterface.h"


struct My_AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightingResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	My_AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, CriticalHitResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, LightingResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UMy_AuraAttributeSet, PhysicalResistance, Target, false);
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
	RelevantAttributesToCapture.Add(My_DamageStatics().CriticalHitResistanceDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().LightingResistanceDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(My_DamageStatics().PhysicalResistanceDef);
}


/**
 * Debuff 判定：在算伤害之前，决定这次攻击有没有触发 Debuff。
 *
 * 为什么不用教程那张 TagsToCaptureDefs（Tag -> CaptureDef）Map：
 *   外面已经逐个抓好了 4 个抗性值（TargetFireResistance 等），
 *   这里直接按抗性 Tag 取对应的那个值就行，不需要再造一张表。
 *
 * 目前只做到「是否触发」这一步（打日志验证）。
 * 等 Context 的 Debuff 字段 + Library 的 Set 函数补齐后，把下面 TODO 那段打开即可。
 */
static void My_DetermineDebuff(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                               const FGameplayEffectSpec& Spec,
                               float InTargetFireResistance,
                               float InTargetLightingResistance,
                               float InTargetArcaneResistance,
                               float InTargetPhysicalResistance)
{
	const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : GameplayTags.DamageToDebuff)
	{
		const FGameplayTag& DamageType = Pair.Key;

		// ① 这个伤害类型这次有没有数据？没有就跳过（说明技能没配这个伤害类型）
		const float TypeDamage = Spec.GetSetByCallerMagnitude(DamageType, false, -1.f);
		if (TypeDamage <= -0.5f) // 用 -0.5 做浮点容差，别写 > -1.f
		{
			continue;
		}

		// ② 施法方配的 Debuff 几率
		const float SourceDebuffChance = Spec.GetSetByCallerMagnitude(GameplayTags.My_Debuff_Chance, false, -1.f);
		if (SourceDebuffChance <= 0.f)
		{
			continue;
		}

		// ③ 被打方对该伤害类型的抗性（复用外面已抓好的值）
		//    用 Find 而不是 operator[]：没配抗性时 operator[] 会断言崩溃
		const FGameplayTag* ResistanceTagPtr = GameplayTags.DamageToResistance.Find(DamageType);
		if (ResistanceTagPtr == nullptr)
		{
			continue;
		}
		const FGameplayTag& ResistanceTag = *ResistanceTagPtr;

		float TargetDebuffResistance = 0.f;
		if (ResistanceTag == GameplayTags.My_Attribute_Secondary_Resistance_Fire)
		{
			TargetDebuffResistance = InTargetFireResistance;
		}
		else if (ResistanceTag == GameplayTags.My_Attribute_Secondary_Resistance_Lighting)
		{
			TargetDebuffResistance = InTargetLightingResistance;
		}
		else if (ResistanceTag == GameplayTags.My_Attribute_Secondary_Resistance_Arcane)
		{
			TargetDebuffResistance = InTargetArcaneResistance;
		}
		else if (ResistanceTag == GameplayTags.My_Attribute_Secondary_Resistance_Physical)
		{
			TargetDebuffResistance = InTargetPhysicalResistance;
		}
		TargetDebuffResistance = FMath::Clamp(TargetDebuffResistance, 0.f, 95.f);

		// ④ 有效几率 = 几率 × (100 - 抗性) / 100
		const float EffectiveDebuffChance = SourceDebuffChance * (100.f - TargetDebuffResistance) / 100.f;

		// ⑤ 掷骰：本次是否触发
		const bool bDebuff = FMath::RandRange(1, 100) < EffectiveDebuffChance;

		UE_LOG(LogTemp, Warning, TEXT("[Debuff判定] 类型=%s  几率=%.1f  抗性=%.1f  有效几率=%.1f  触发=%s"),
			*DamageType.ToString(), SourceDebuffChance, TargetDebuffResistance, EffectiveDebuffChance,
			bDebuff ? TEXT("是") : TEXT("否"));

		// ===== TODO：等 Context 的 Debuff 字段 + My_AuraAbilitySystemLibrary 的 Set 函数补齐后打开 =====
		// if (bDebuff)
		// {
		// 	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
		//
		// 	UMy_AuraAbilitySystemLibrary::SetIsSuccessfulDebuff(ContextHandle, true);
		//
		// 	const float DebuffDamage    = Spec.GetSetByCallerMagnitude(GameplayTags.My_Debuff_Damage,    false, -1.f);
		// 	const float DebuffDuration  = Spec.GetSetByCallerMagnitude(GameplayTags.My_Debuff_Duration,  false, -1.f);
		// 	const float DebuffFrequency = Spec.GetSetByCallerMagnitude(GameplayTags.My_Debuff_Frequency, false, -1.f);
		//
		// 	UMy_AuraAbilitySystemLibrary::SetDebuffDamage(ContextHandle, DebuffDamage);
		// 	UMy_AuraAbilitySystemLibrary::SetDebuffDuration(ContextHandle, DebuffDuration);
		// 	UMy_AuraAbilitySystemLibrary::SetDebuffFrequency(ContextHandle, DebuffFrequency);
		// }
	}
}


void UMy_ExeCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 获取源和目标的能力系统组件及对应的Avatar Actor
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	// 获取战斗接口用于获取角色等级等信息
	int32 SourceLevel = 1;
	if (SourceAvatar->Implements<UMy_CombatInterface>())
	{
		SourceLevel = IMy_CombatInterface::Execute_GetPlayerLevel(SourceAvatar);
	}
	int32 TargetLevel = 1;
	if (TargetAvatar->Implements<UMy_CombatInterface>())
	{
		TargetLevel = IMy_CombatInterface::Execute_GetPlayerLevel(TargetAvatar);
	}

	// 获取效果规格和评估参数
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// ===== 获取所有相关属性 =====
	// 基础伤害值
	float Damage = 0.f;


	// 防御方属性
	float TargetBlockChance = 0.f; // 格挡几率
	float TargetArmor = 0.f; // 护甲值
	float TargetCriticalHitResistance = 0.f; // 暴击抗性
	float TargetFireResistance = 0.f; // 火焰抗性
	float TargetLightingResistance = 0.f; // 雷电抗性
	float TargetArcaneResistance = 0.f; // 奥术抗性
	float TargetPhysicalResistance = 0.f; // 物理抗性
	// 攻击方属性
	float SourceArmorPenetration = 0.f; // 护甲穿透
	float SourceCriticalHitChance = 0.f; // 暴击几率
	float SourceCriticalHitDamage = 0.f; // 暴击伤害倍率

	// 从AbilitySystemComponent捕获属性值
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().BlockChanceDef, EvaluateParameters, TargetBlockChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().ArmorDef, EvaluateParameters, TargetArmor);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().CriticalHitResistanceDef, EvaluateParameters, TargetCriticalHitResistance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().ArmorPenetrationDef, EvaluateParameters, SourceArmorPenetration);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().CriticalHitChanceDef, EvaluateParameters, SourceCriticalHitChance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().CriticalHitDamageDef, EvaluateParameters, SourceCriticalHitDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().FireResistanceDef, EvaluateParameters, TargetFireResistance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().LightingResistanceDef, EvaluateParameters, TargetLightingResistance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().ArcaneResistanceDef, EvaluateParameters, TargetArcaneResistance);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(My_DamageStatics().PhysicalResistanceDef, EvaluateParameters, TargetPhysicalResistance);

	// 确保所有属性值非负
	TargetBlockChance = FMath::Max(0.f, TargetBlockChance);
	TargetArmor = FMath::Max(0.f, TargetArmor);
	TargetCriticalHitResistance = FMath::Max(0.f, TargetCriticalHitResistance);
	SourceArmorPenetration = FMath::Max(0.f, SourceArmorPenetration);
	SourceCriticalHitChance = FMath::Max(0.f, SourceCriticalHitChance);
	SourceCriticalHitDamage = FMath::Max(1.f, SourceCriticalHitDamage); // 暴击伤害至少为1倍
	TargetFireResistance = FMath::Max(0.f, TargetFireResistance);
	TargetLightingResistance = FMath::Max(0.f, TargetLightingResistance);
	TargetArcaneResistance = FMath::Max(0.f, TargetArcaneResistance);
	TargetPhysicalResistance = FMath::Max(0.f, TargetPhysicalResistance);

	// ===== Debuff 判定（放在算伤害之前；复用上面抓好的抗性值，不需要额外的 Tag->Def 映射表）=====
	My_DetermineDebuff(ExecutionParams, Spec,
		TargetFireResistance, TargetLightingResistance, TargetArcaneResistance, TargetPhysicalResistance);

	// ===== 获取伤害计算系数 =====
	UMy_CharacterClassInfo* CharacterClassInfo = UMy_AuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());

	// 根据角色等级获取曲线值
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceLevel);
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetLevel);
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetLevel);


	// ===== 伤害计算流程 =====
	// 1. 抗性计算
	for (const auto& Pair : FMy_AuraGameplayTags::GetInstance().DamageToResistance)
	{
		// 从这里获取之前设置的 Set By Caller 数值
		float DamageValue = Spec.GetSetByCallerMagnitude(Pair.Key, false, 0.f);
		float ResistanceValue = 0.f;
		if (Pair.Value == FMy_AuraGameplayTags::GetInstance().My_Attribute_Secondary_Resistance_Fire)
		{
			ResistanceValue = TargetFireResistance;
		}
		else if (Pair.Value == FMy_AuraGameplayTags::GetInstance().My_Attribute_Secondary_Resistance_Lighting)
		{
			ResistanceValue = TargetLightingResistance;
		}
		else if (Pair.Value == FMy_AuraGameplayTags::GetInstance().My_Attribute_Secondary_Resistance_Arcane)
		{
			ResistanceValue = TargetArcaneResistance;
		}
		else if (Pair.Value == FMy_AuraGameplayTags::GetInstance().My_Attribute_Secondary_Resistance_Physical)
		{
			ResistanceValue = TargetPhysicalResistance;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Unknown damage resistance type: %s"), *Pair.Value.ToString());
		}
		ResistanceValue = FMath::Clamp(ResistanceValue, 0.f, 95.f);
		Damage += DamageValue * (100.f - ResistanceValue) / 100.f;
	}

	// 2. 格挡判定
	const bool bIsBlocked = FMath::RandRange(0.f, 1.f) <= TargetBlockChance;
	UMy_AuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bIsBlocked);
	if (bIsBlocked)
	{
		// 格挡成功，伤害减半
		Damage *= 0.5f;
	}
	UE_LOG(LogTemp, Warning, TEXT("Block: %s"), bIsBlocked ? TEXT("true") : TEXT("false"));

	// 3. 护甲穿透计算
	// 计算实际护甲穿透效果(限制在0-1范围内)
	const float EffectivePenetration = FMath::Clamp(SourceArmorPenetration * ArmorPenetrationCoefficient, 0.f, 1.f);
	// 计算剩余有效护甲
	const float EffectiveArmor = (1 - EffectivePenetration) * TargetArmor;

	// 4. 护甲减伤计算
	// 计算护甲提供的伤害减免百分比(限制在0%-95%之间，避免完全免疫)
	const float ArmorDamageReduction = FMath::Clamp(EffectiveArmor * EffectiveArmorCoefficient, 0.f, 95.f);
	// 应用护甲减伤
	Damage *= (100 - ArmorDamageReduction) / 100.f;

	// 5. 暴击判定与计算 
	if (!bIsBlocked) // 通常格挡攻击不会暴击
	{
		// 计算实际暴击几率(考虑目标的暴击抗性)
		const float EffectiveCritChance = FMath::Max(0.f, SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient);
		UE_LOG(LogTemp, Warning, TEXT("EffectiveCritChance: %f"), EffectiveCritChance);

		// 是否暴击
		const bool bIsCritical = FMath::RandRange(0.f, 1.f) < EffectiveCritChance;
		UMy_AuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bIsCritical);
		if (bIsCritical)
		{
			// 应用暴击伤害倍率
			Damage *= SourceCriticalHitDamage;
			UE_LOG(LogTemp, Warning, TEXT("Critical Hit! Damage: %f"), Damage);
		}
		UE_LOG(LogTemp, Warning, TEXT("Critical Hit: %s"), bIsCritical ? TEXT("true") : TEXT("false"));
	}

	// ===== 输出最终伤害值 =====
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		UMy_AuraAttributeSet::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive,
		Damage
	));
}
