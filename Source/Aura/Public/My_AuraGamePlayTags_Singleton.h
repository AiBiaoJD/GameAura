// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton Containing native Gameplay Tags
 */

struct FMy_AuraGameplayTags
{
public:
	static const FMy_AuraGameplayTags& GetInstance() { return GameplayTagsInstance; } //全局访问入口，const引用防拷贝
	static void InitializeNativeGameplayTags(); //静态方法，操作静态实例

	// ===== 一、属性 Attribute =====

	// 主属性
	FGameplayTag My_Attribute_Primary_Strength;
	FGameplayTag My_Attribute_Primary_Intelligence;
	FGameplayTag My_Attribute_Primary_Resilience;
	FGameplayTag My_Attribute_Primary_VIgor;

	// 次属性
	FGameplayTag My_Attribute_Secondary_Armor;
	FGameplayTag My_Attribute_Secondary_ArmorPenetration;
	FGameplayTag My_Attribute_Secondary_HealthRegeneration;
	FGameplayTag My_Attribute_Secondary_ManaRegeneration;
	FGameplayTag My_Attribute_Secondary_MaxMana;
	FGameplayTag My_Attribute_Secondary_MaxHealth;
	FGameplayTag My_Attribute_Secondary_BlockChance;
	FGameplayTag My_Attribute_Secondary_CriticalHitResistance;
	FGameplayTag My_Attribute_Secondary_CriticalHitChance;
	FGameplayTag My_Attribute_Secondary_CriticalHitDamage;
	FGameplayTag My_Attribute_Secondary_Resistance_Fire;
	FGameplayTag My_Attribute_Secondary_Resistance_Lighting;
	FGameplayTag My_Attribute_Secondary_Resistance_Arcane;
	FGameplayTag My_Attribute_Secondary_Resistance_Physical;

	// 元属性（不直接参与战斗计算）
	FGameplayTag My_Attribute_Meta_IncomingXP;
	
	// ===== 二、输入 Input =====

	// 主动技能槽位
	FGameplayTag My_InputTag_LMB;
	FGameplayTag My_InputTag_RMB;
	FGameplayTag My_InputTag_1;
	FGameplayTag My_InputTag_2;
	FGameplayTag My_InputTag_3;
	FGameplayTag My_InputTag_4;
	FGameplayTag My_InputTag_5;

	// 被动技能槽位
	FGameplayTag My_InputTag_Passive_1;
	FGameplayTag My_InputTag_Passive_2;


	// ===== 三、伤害与减益 Damage / Debuff =====

	// 伤害类型
	FGameplayTag My_DamageType_Fire;
	FGameplayTag My_DamageType_Lighting;
	FGameplayTag My_DamageType_Arcane;
	FGameplayTag My_DamageType_Physical;

	// 减益类型
	FGameplayTag My_Debuff_Burn;
	FGameplayTag My_Debuff_Stun;
	FGameplayTag My_Debuff_Arcane;
	FGameplayTag My_Debuff_Physical;

	// 减益参数（GE 的 SetByCaller）
	FGameplayTag My_Debuff_Chance;
	FGameplayTag My_Debuff_Damage;
	FGameplayTag My_Debuff_Frequency;
	FGameplayTag My_Debuff_Duration;

	// 伤害数值（GE 的 SetByCaller）
	FGameplayTag My_EffectData_Damage;

	// GE 授予的标签
	FGameplayTag My_EffectGranted_HitReact;
	
	// ===== 四、技能标识 Ability =====
	// 这一层回答「是哪个技能」：写进 GA 的 AbilityTags，也是技能树的 AbilityTag

	// ---- 4.1 技能树内：会出现在 SpellTree，玩家可解锁 / 装备 ----

	// 主动技能
	FGameplayTag My_Abilities_Fire_FireBolt;
	FGameplayTag My_Abilities_Lighting_Electrocute;
	FGameplayTag My_Abilities_Arcane_ArcaneShards;

	// 被动技能
	FGameplayTag My_Abilities_Passive_HaloOfProtection;
	FGameplayTag My_Abilities_Passive_LifeSiphon;
	FGameplayTag My_Abilities_Passive_ManaSiphon;

	// ---- 4.2 技能树外：通用能力，不进技能树、不占玩家技能槽 ----

	FGameplayTag My_Abilities_Attack;    // 近战普攻，绑 LMB
	FGameplayTag My_Abilities_Summon;    // 敌人召唤
	FGameplayTag My_Abilities_HitReact;  // 受击，暂时没使用

	// ---- 4.3 空技能哨兵 ----
	// 表示"这个槽位没有技能"，等价于 AbilityTag 的 nullptr。
	// 取消选中 / 蓝图里没配 AbilityTag 时用它占位。
	// 判断用：Tag.MatchesTag(My_Abilities_None)
	FGameplayTag My_Abilities_None;


	// ===== 五、技能状态与类型 Status / Type =====

	// 状态：技能树里每个技能当前处于哪一档
	FGameplayTag My_Abilities_Status_Locked;
	FGameplayTag My_Abilities_Status_Eligible;
	FGameplayTag My_Abilities_Status_Unlocked;
	FGameplayTag My_Abilities_Status_Equipped;

	// 类型：技能分类（攻击型 / 被动型）
	FGameplayTag My_Abilities_Type_Offensive;
	FGameplayTag My_Abilities_Type_Passive;
	// 空类型哨兵：与 4.3 的 None 是两层，别混用。
	// None 比的是"是哪个技能"，Type_None 比的是"是哪类技能"。当前项目尚未使用。
	FGameplayTag My_Abilities_Type_None;

	
	// ===== 六、冷却 Cooldown =====
	// 与技能一一对应，GE_Cooldown_XXX 用 SetByCaller 读它

	FGameplayTag My_CoolDown_Fire_FireBolt;
	FGameplayTag My_CoolDown_Arcane_ArcaneShards;


	// ===== 七、动画与插槽 Montage / Socket =====

	// 武器插槽
	FGameplayTag My_CombatSocket_Weapon;
	FGameplayTag My_CombatSocket_LeftHand;
	FGameplayTag My_CombatSocket_RightHand;
	FGameplayTag My_CombatSocket_Tail;

	// 近战连击蒙太奇
	FGameplayTag My_Montage_Attack1;
	FGameplayTag My_Montage_Attack2;
	FGameplayTag My_Montage_Attack3;
	FGameplayTag My_Montage_Attack4;


	// ===== 八、玩家状态 Player =====

	FGameplayTag My_Player_Block_InputPressed;
	FGameplayTag My_Player_Block_InputReleased;
	FGameplayTag My_Player_Block_InputHeld;
	FGameplayTag My_Player_Block_CursorTrace;
	FGameplayTag My_Player_Status_ShockLoop;

	
	TMap<FGameplayTag, FGameplayTag> DamageToResistance;
	TMap<FGameplayTag, FGameplayTag> DamageToDebuff;


private:
	static FMy_AuraGameplayTags GameplayTagsInstance; // C++：声明，必须在这里
};
