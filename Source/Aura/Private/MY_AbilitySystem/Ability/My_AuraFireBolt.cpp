// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraFireBolt.h"

#include "My_AuraGamePlayTags_Singleton.h"

FString UMy_AuraFireBolt::GetDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageTag(Level, FMy_AuraGameplayTags::GetInstance().My_DamageType_Fire);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		// 1 级专属文案：发射数写死为 1，并额外提示升级后的成长
		return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>1</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>1</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>\n\n<Small>升级后可同时发射更多焰矢。</>"), ManaCost, Cooldown, Damage);
	}

	// 高等级：发射数随等级增加，上限为 NumProjectiles
	const int32 NumProj = FMath::Min(Level, NumProjectiles);
	return FString::Printf(TEXT("<Title>焰矢</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>"), Level, ManaCost, Cooldown, NumProj, Damage);
}

FString UMy_AuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const int32 Damage = GetDamageByDamageTag(Level, FMy_AuraGameplayTags::GetInstance().My_DamageType_Fire);
	const int32 NumProj = FMath::Min(Level, NumProjectiles);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT("<Title>下一等级</>\n\n<Small>当前等级 </><Level>%d</>\n<Small>消耗蓝量 </><ManaCost>%.1f</>\n<Small>冷却时间 </><CoolDown>%.1f</>\n\n<Default>发射 </><Level>%d</><Default> 枚焰矢，撞击目标时爆炸，造成 </><Damage>%d</><Default> 点火焰伤害，并有几率使目标灼烧。</>"), Level, ManaCost, Cooldown, NumProj, Damage);
}
