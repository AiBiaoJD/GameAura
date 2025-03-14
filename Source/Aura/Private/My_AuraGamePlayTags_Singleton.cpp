// Copyright ABiao


#include "My_AuraGamePlayTags_Singleton.h"

#include "GameplayTagsManager.h"


FMy_AuraGameplayTags FMy_AuraGameplayTags::GameplayTagsInstance;


void FMy_AuraGameplayTags::InitializeNativeGameplayTags()
{
    // Strength: Increases physical damage.
    GameplayTagsInstance.My_Attribute_Primary_Strength = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Strength"), FString("Increases physical damage."));

    // Intelligence: Increases magical damage.
    GameplayTagsInstance.My_Attribute_Primary_Intelligence = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Intelligence"), FString("Increases magical damage."));

    // Resilience: Increases defense and reduces damage taken.
    GameplayTagsInstance.My_Attribute_Primary_Resilience = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.Resilience"), FString("Increases defense and reduces damage taken."));

    // Vigor: Increases health and related attributes.
    GameplayTagsInstance.My_Attribute_Primary_VIgor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Primary.VIgor"), FString("Increases health and related attributes."));




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
}