// Copyright ABiao


#include "My_AbilitySystem/Ability/My_ArcaneShards.h"

FString UMy_ArcaneShards::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		// 1 级专属文案：点位数写死为 1，并额外提示升级后的成长
		return FString::Printf(TEXT("<Title>奥术碎片</>\n\n<Small>当前等级 </><Level>1</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>在目标位置召唤 </><Level>1</><Default> 个奥术点位，每个点位对周围敌人造成 </><Damage>%d</><Default> 点奥术范围伤害。</>\n\n<Small>升级后可同时召唤更多点位。</>"), ManaCost, Cooldown, ScaledDamage);
	}

	// 高等级：点位数量随等级增加，等级 N 就是 N 个点
	return FString::Printf(TEXT("<Title>奥术碎片</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>在目标位置召唤 </><Level>%d</><Default> 个奥术点位，每个点位对周围敌人造成 </><Damage>%d</><Default> 点奥术范围伤害。</>"), Level, ManaCost, Cooldown, Level, ScaledDamage);
}

FString UMy_ArcaneShards::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>在目标位置召唤 </><Level>%d</><Default> 个奥术点位，每个点位对周围敌人造成 </><Damage>%d</><Default> 点奥术范围伤害。</>"), Level, ManaCost, Cooldown, Level, ScaledDamage);
}
