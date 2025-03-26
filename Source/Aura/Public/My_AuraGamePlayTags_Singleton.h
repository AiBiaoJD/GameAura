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
	static const FMy_AuraGameplayTags& GetInstance() { return GameplayTagsInstance; }
	static void InitializeNativeGameplayTags();

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


private:
	static FMy_AuraGameplayTags GameplayTagsInstance;
};
