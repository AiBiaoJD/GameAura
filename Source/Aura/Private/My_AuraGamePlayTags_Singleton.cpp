// Copyright ABiao


#include "My_AuraGamePlayTags_Singleton.h"

#include "GameplayTagsManager.h"


FMy_AuraGameplayTags FMy_AuraGameplayTags::GameplayTagsInstance; //C++：定义，才分配内存，不能省


void FMy_AuraGameplayTags::InitializeNativeGameplayTags()
{
	/*
	 * Primary Attribute Tag
	 */
	// Strength: Increases physical damage.
	GameplayTagsInstance.My_Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Strength"), FString("Increases physical damage."));

	// Intelligence: Increases magical damage.
	GameplayTagsInstance.My_Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Intelligence"), FString("Increases magical damage."));

	// Resilience: Increases defense and reduces damage taken.
	GameplayTagsInstance.My_Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Resilience"), FString("Increases defense and reduces damage taken."));

	// Vigor: Increases health and related attributes.
	GameplayTagsInstance.My_Attribute_Primary_VIgor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.VIgor"), FString("Increases health and related attributes."));


	/*
	 * SecondaryAttribute Tag
	 */
	// Armor: Reduces damage taken by a percentage and increases Block Chance (halves damage taken) and reduces critical damage from enemies.
	GameplayTagsInstance.My_Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.Armor"), FString("Reduces damage taken by a percentage and increases Block Chance (halves damage taken) and reduces critical damage from enemies."));

	// Armor Penetration: Ignores a portion of the enemy's armor and increases critical hit chance.
	GameplayTagsInstance.My_Attribute_Secondary_ArmorPenetration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.ArmorPenetration"), FString("Ignores a portion of the enemy's armor and increases critical hit chance."));

	// Health Regeneration: Automatically regenerates health per second.
	GameplayTagsInstance.My_Attribute_Secondary_HealthRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.HealthRegeneration"), FString("Automatically regenerates health per second."));

	// Mana Regeneration: Automatically regenerates mana per second.
	GameplayTagsInstance.My_Attribute_Secondary_ManaRegeneration = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.ManaRegeneration"), FString("Automatically regenerates mana per second."));

	// Max Health: Maximum health value, controlled by Vigor.
	GameplayTagsInstance.My_Attribute_Secondary_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.MaxHealth"), FString("Maximum health value, controlled by Vigor."));

	// Max Mana: Maximum mana value, controlled by Intelligence.
	GameplayTagsInstance.My_Attribute_Secondary_MaxMana = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.MaxMana"), FString("Maximum mana value, controlled by Intelligence."));

	// Block Chance: Chance to halve incoming damage.
	GameplayTagsInstance.My_Attribute_Secondary_BlockChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.BlockChance"), FString("Chance to halve incoming damage."));

	// Critical Hit Resistance: Reduces critical damage taken by a percentage.
	GameplayTagsInstance.My_Attribute_Secondary_CriticalHitResistance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.CriticalHitResistance"), FString("Reduces critical damage taken by a percentage."));

	// Critical Hit Chance: Chance to deal double damage.
	GameplayTagsInstance.My_Attribute_Secondary_CriticalHitChance = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.CriticalHitChance"), FString("Chance to deal double damage."));

	// Critical Hit Damage: Increases damage dealt by critical hits.
	GameplayTagsInstance.My_Attribute_Secondary_CriticalHitDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.CriticalHitDamage"), FString("Increases damage dealt by critical hits."));

	/*
	* Input Tag
	*/

	GameplayTagsInstance.My_InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.LMB"), FString("Left Mouse Button Input."));
	GameplayTagsInstance.My_InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.RMB"), FString("Right Mouse Button Input."));
	GameplayTagsInstance.My_InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.1"), FString("Input for Key 1."));
	GameplayTagsInstance.My_InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.2"), FString("Input for Key 2."));
	GameplayTagsInstance.My_InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.3"), FString("Input for Key 3."));
	GameplayTagsInstance.My_InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.4"), FString("Input for Key 4."));
	GameplayTagsInstance.My_InputTag_5 = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_InputTag.5"), FString("Input for Key 5."));

	/*
	* Effect SetbyCaller Tag
	*/
	GameplayTagsInstance.My_EffectData_Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_EffectData.Damage"), FString("Effect Cause Damage"));

	/*
	* Effect Granted Tag
	*/

	GameplayTagsInstance.My_EffectGranted_HitReact = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_EffectGranted.HitReact"), FString("Tag Granted When Hit Reaction"));


	/*
	 * DamageType Tags
	 */
	GameplayTagsInstance.My_DamageType_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_DamageType.Fire"),
		FString("Fire elemental damage type")
	);

	GameplayTagsInstance.My_DamageType_Lighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_DamageType.Lighting"),
		FString("Lighting elemental damage type")
	);

	GameplayTagsInstance.My_DamageType_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_DamageType.Arcane"),
		FString("Arcane magical damage type")
	);

	GameplayTagsInstance.My_DamageType_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_DamageType.Physical"),
		FString("Physical blunt/sharp damage type")
	);

	/*
	 * Resistance Tags
	 */
	GameplayTagsInstance.My_Attribute_Secondary_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Attribute.Secondary.Resistance.Fire"),
		FString("Reduces incoming fire damage")
	);

	GameplayTagsInstance.My_Attribute_Secondary_Resistance_Lighting = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Attribute.Secondary.Resistance.Lighting"),
		FString("Reduces incoming lighting damage")
	);

	GameplayTagsInstance.My_Attribute_Secondary_Resistance_Arcane = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Attribute.Secondary.Resistance.Arcane"),
		FString("Reduces incoming arcane damage")
	);

	GameplayTagsInstance.My_Attribute_Secondary_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Attribute.Secondary.Resistance.Physical"),
		FString("Reduces incoming physical damage")
	);


	/*
	* Abilities Tags
	*/
	GameplayTagsInstance.My_Attribute_Secondary_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Abilities.Attack;"),
		FString("Attack Melee Tag")
	);

	/*
	* Combat Socket
	*/
	GameplayTagsInstance.My_CombatSocket_Weapon = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_CombatSocket.Weapon"),
		FString("Attack By Weapon")
	);
	GameplayTagsInstance.My_CombatSocket_LeftHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_CombatSocket.LeftHand"),
		FString("Attack By LeftHand")
	);
	GameplayTagsInstance.My_CombatSocket_RightHand = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_CombatSocket.RightHand"),
		FString("Attack By RightHand")
	);

	/*
	* Montage Tag
	*/
	GameplayTagsInstance.My_Montage_Attack1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Montage.Attack.1"),
		FString("Montage Attack 1")
	);
	GameplayTagsInstance.My_Montage_Attack2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Montage.Attack.2"),
		FString("Montage Attack 2")
	);
	GameplayTagsInstance.My_Montage_Attack3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Montage.Attack.3"),
		FString("Montage Attack 3")
	);
	GameplayTagsInstance.My_Montage_Attack4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("My_Montage.Attack.4"),
		FString("Montage Attack 4")
	);

	// Map: DamageType To  Resistance
	GameplayTagsInstance.DamageToResistance.Add(GameplayTagsInstance.My_DamageType_Fire, GameplayTagsInstance.My_Attribute_Secondary_Resistance_Fire);
	GameplayTagsInstance.DamageToResistance.Add(GameplayTagsInstance.My_DamageType_Lighting, GameplayTagsInstance.My_Attribute_Secondary_Resistance_Lighting);
	GameplayTagsInstance.DamageToResistance.Add(GameplayTagsInstance.My_DamageType_Arcane, GameplayTagsInstance.My_Attribute_Secondary_Resistance_Arcane);
	GameplayTagsInstance.DamageToResistance.Add(GameplayTagsInstance.My_DamageType_Physical, GameplayTagsInstance.My_Attribute_Secondary_Resistance_Physical);
}
