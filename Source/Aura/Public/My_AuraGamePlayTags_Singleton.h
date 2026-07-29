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

	// Primary Attributes Tag
	FGameplayTag My_Attribute_Primary_Strength;
	FGameplayTag My_Attribute_Primary_Intelligence;
	FGameplayTag My_Attribute_Primary_Resilience;
	FGameplayTag My_Attribute_Primary_VIgor;


	// Secondary Attributes Tag
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

	// Meta Attributes Tag
	FGameplayTag My_Attribute_Meta_IncomingXP;

	
	// InputActions Tag
	FGameplayTag My_InputTag_LMB;
	FGameplayTag My_InputTag_RMB;
	FGameplayTag My_InputTag_1;
	FGameplayTag My_InputTag_2;
	FGameplayTag My_InputTag_3;
	FGameplayTag My_InputTag_4;
	FGameplayTag My_InputTag_5;

	// Effect SetbyCaller Tag
	FGameplayTag My_EffectData_Damage;

	// Effect Granted Tag
	FGameplayTag My_EffectGranted_HitReact;

	// DamageType Tag
	FGameplayTag My_DamageType_Fire;
	FGameplayTag My_DamageType_Lighting;
	FGameplayTag My_DamageType_Arcane;
	FGameplayTag My_DamageType_Physical;

	// Attack
	FGameplayTag My_Abilities_Attack;
	FGameplayTag My_Abilities_Summon;
	FGameplayTag My_Abilities_Fire_FireBolt;

	//CoolDown
	FGameplayTag My_CoolDown_Fire_FireBolt;

	// Montage
	FGameplayTag My_CombatSocket_Weapon;
	FGameplayTag My_CombatSocket_LeftHand;
	FGameplayTag My_CombatSocket_RightHand;
	FGameplayTag My_CombatSocket_Tail;

	FGameplayTag My_Montage_Attack1;
	FGameplayTag My_Montage_Attack2;
	FGameplayTag My_Montage_Attack3;
	FGameplayTag My_Montage_Attack4;

	
	// Map: DamageType To  Resistance
	TMap<FGameplayTag,FGameplayTag> DamageToResistance;

private:
	static FMy_AuraGameplayTags GameplayTagsInstance; // C++：声明，必须在这里
};
