// Copyright ABiao


#include "My_AbilitySystem/Ability/My_AuraFireBlast.h"

FString UMy_AuraFireBlast::GetDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>火焰爆裂</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，贴近自身时爆炸造成径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage, DebuffChance);
}

FString UMy_AuraFireBlast::GetNextLevelDescription(int32 Level)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>向四周甩出 </><Level>%d</><Default> 颗火球，火球飞出后掉头飞回，回程撞击敌人造成 </><Damage>%d</><Default> 点火焰伤害，贴近自身时爆炸造成径向伤害，并有 </><Level>%.0f%%</><Default> 的几率使目标灼烧。</>"),
		Level, ManaCost, Cooldown, NumFireBalls, ScaledDamage, DebuffChance);
}
