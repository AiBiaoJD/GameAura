// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "My_AuraAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// typedef is specific to the FgameplayAttribute(), But TStaticFuncPtr is general to any static

// typedef  TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultTSDelegateUserPolicy>::FFuncPtr FMy_AttributeFuncPtr;
template <class T>
using TMy_StaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultTSDelegateUserPolicy>::FFuncPtr;


//保存PostGameplayEffectExecute()调用后里面的数据，包括Effect的Source和Target等
USTRUCT()
struct FMy_EffectProperties
{
	GENERATED_BODY()
	FMy_EffectProperties()
	{
	}

	//Effect上下文
	FGameplayEffectContextHandle EffectContextHandle;

	//Source
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	//Target
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};


/**
 *
 */
UCLASS()
class AURA_API UMy_AuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMy_AuraAttributeSet();
	/*
	1.定义哪些属性需要在网络上复制
	2.每个属性设置特定的复制条件和规则
	3.通过精确控制需要复制的属性，可以优化网络带宽使用
	*/
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;


	//Tag 和 函数指针

	TMap<FGameplayTag, TMy_StaticFuncPtr<FGameplayAttribute()>> M_TagsToAttribute;


	/*
	 *Primary Attribute:主要属性
	 */

	//力量:提升物理攻击力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attribute")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Strength);

	//智力:提升法术攻击力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attribute")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Intelligence);

	//抗性:提高防御减免和防御穿透
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attribute")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Resilience);

	//活力:提高生命值等相关
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_VIgor, Category = "Primary Attribute")
	FGameplayAttributeData VIgor;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, VIgor);


	/*
	 *Second Attribute;二级属性
	 */

	//----------1.基于抗性Resilience属性生成----------

	//护甲:主要减少受到的伤害百分比,并提高Block Chance的概率(受到伤害减半)和敌人的暴击伤害
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Armor, Category = "Second Attribute")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Armor);


	//护甲穿透:攻击时忽略敌人部分护甲值,并提高暴击几率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Second Attribute")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, ArmorPenetration);

	// 火焰抗性：减少受到的火焰伤害
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Second Attribute")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, FireResistance);

	// 雷电抗性：减少受到的雷电伤害  
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LightingResistance, Category = "Second Attribute")
	FGameplayAttributeData LightingResistance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, LightingResistance);

	// 奥术抗性：减少受到的奥术伤害
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArcaneResistance, Category = "Second Attribute")
	FGameplayAttributeData ArcaneResistance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, ArcaneResistance);

	// 物理抗性：减少受到的物理伤害
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Second Attribute")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, PhysicalResistance);

	//----------2.基于活力Vigor属性生成----------

	//每秒治疗量:每秒自动回复值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_HealthRegeneration, Category = "Second Attribute")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, HealthRegeneration);

	//Max血量:也是二级属性受Vigor控制
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attribute")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxHealth);

	//----------3.基于智力intelligence属性生成----------

	//每秒蓝量回复值:每秒蓝量自动回复值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ManaRegeneration, Category = "Second Attribute")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, ManaRegeneration);

	//Max蓝量:也是二级属性受Intelligence控制
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxMana);


	/*
	 *Third  Attribute;三级属性
	 */

	//----------1.基于护甲Armor属性生成----------

	//伤害减半几率:受到伤害减半的概率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BlockChance, Category = "Third Attribute")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, BlockChance);

	//暴击抗性:降低敌人暴击的概率或暴击伤害
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitResistance, Category = "Third Attribute")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, CriticalHitResistance);

	//----------2.基于护甲穿透ArmorPenetration属性生成----------

	//暴击几率:造成2倍伤害的几率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitChance, Category = "Third Attribute")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, CriticalHitChance);

	//暴击伤害:造成2倍伤害+百分比
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalHitDamage, Category = "Third Attribute")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, CriticalHitDamage);


	/*
	* Vital Attributes:核心属性
	*/

	//ReplicatedUsing = OnRep_Health,属性更新时执行回调函数
	//血量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attribute")
	FGameplayAttributeData Health;
	//宏定义
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Health);

	//蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attribute")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Mana);

	/*
	 * MetaAttribute:只在服务器上运行,不会Replicated到客户端
	 */
	UPROPERTY(BlueprintReadOnly, Category="Meta Attribute")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, IncomingDamage);

	UPROPERTY(BlueprintReadOnly, Category="Meta Attribute")
	FGameplayAttributeData IncomingXP;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, IncomingXP);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRep_VIgor(const FGameplayAttributeData& OldVIgor) const;

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	UFUNCTION()
	void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UFUNCTION()
	void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UFUNCTION()
	void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	UFUNCTION()
	void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	UFUNCTION()
	void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;

	UFUNCTION()
	void OnRep_LightingResistance(const FGameplayAttributeData& OldLightingResistance) const;

	UFUNCTION()
	void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;

	UFUNCTION()
	void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;

private:
	void SetEffectProperty(const struct FGameplayEffectModCallbackData& Data, FMy_EffectProperties& Props) const;
	void ShowDamageText(const FMy_EffectProperties& Props, float Damage, bool IsBlockedHit, bool IsCriticalHit) const;
	void SendXPEvent(const FMy_EffectProperties& Props) const;
};
