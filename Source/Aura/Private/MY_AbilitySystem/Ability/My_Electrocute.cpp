// Copyright ABiao


#include "My_AbilitySystem/Ability/My_Electrocute.h"

FString UMy_Electrocute::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		// 1 级专属文案：只有主目标，不连锁
		return FString::Printf(TEXT("<Title>电击</>\n\n<Small>当前等级 </><Level>1</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>射出一道闪电持续电击目标，每次造成 </><Damage>%d</><Default> 点闪电伤害，并有 </><Level>%.0f%%</><Default> 的基础几率使目标眩晕。</>\n\n<Small>升级后会从主目标连锁到附近的敌人。</>"), ManaCost, Cooldown, ScaledDamage, DebuffChance);
	}

	// 高等级：额外连锁数 = 等级 - 1（等级 2 连锁 1 个，等级 3 连锁 2 个），上限为 MaxNumShockTargets
	const int32 AdditionalTargets = FMath::Min(Level - 1, MaxNumShockTargets);
	return FString::Printf(TEXT("<Title>电击</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>射出一道闪电持续电击主目标，并从主目标额外连锁 </><Level>%d</><Default> 个附近敌人，每次造成 </><Damage>%d</><Default> 点闪电伤害，并有 </><Level>%.0f%%</><Default> 的基础几率使目标眩晕。</>"), Level, ManaCost, Cooldown, AdditionalTargets, ScaledDamage, DebuffChance);
}

FString UMy_Electrocute::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	const int32 AdditionalTargets = FMath::Min(Level - 1, MaxNumShockTargets);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>射出一道闪电持续电击主目标，并从主目标额外连锁 </><Level>%d</><Default> 个附近敌人，每次造成 </><Damage>%d</><Default> 点闪电伤害，并有 </><Level>%.0f%%</><Default> 的基础几率使目标眩晕。</>"), Level, ManaCost, Cooldown, AdditionalTargets, ScaledDamage, DebuffChance);
}
