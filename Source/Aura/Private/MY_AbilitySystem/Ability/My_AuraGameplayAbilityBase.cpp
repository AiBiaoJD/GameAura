// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"

#include "MY_AbilitySystem/My_AuraAttributeSet.h"

FString UMy_AuraGameplayAbilityBase::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default> %s, </><Level>%d</>"), TEXT("默认能力名字 -LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum"), Level);
}

FString UMy_AuraGameplayAbilityBase::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>NextLevel:</><Level>%d</> \n<Default>造成更多伤害</>"), Level);
}

FString UMy_AuraGameplayAbilityBase::GetLockDescription(int32 LevelRequirement)
{
	return FString::Printf(TEXT("<Default>技能未解锁,需要等级:</><Level>%d</>"), LevelRequirement);
}

float UMy_AuraGameplayAbilityBase::GetManaCost(int32 InLevel) const
{
	float ManaCost = 0.f;
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		for (FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			if (Mod.Attribute == UMy_AuraAttributeSet::GetManaAttribute())
			{
				// 从 GE 的修改器里取出"写在数据里的"数值（仅 ScalableFloat 类型可用）
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UMy_AuraGameplayAbilityBase::GetCooldown(int32 InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		// 冷却时间写在 GE 的 DurationMagnitude 上，而不是 Modifiers 里
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
