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
	ACharacter* SourceCharacter= nullptr;

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

	/*
	 *Primary Attribute
	 */

	//力量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "Primary Attribute")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Strength);

	//智力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "Primary Attribute")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Intelligence);

	//伤害抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Resilience, Category = "Primary Attribute")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Resilience);

	//增强生命值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_VIgor, Category = "Primary Attribute")
	FGameplayAttributeData VIgor;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, VIgor);

	/*
	 *Vital Attributes
	 */

	//ReplicatedUsing = OnRep_Health,属性更新时执行回调函数
	//血量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attribute")
	FGameplayAttributeData Health;
	//宏定义
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Health);

	//Max血量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attribute")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxHealth);


	//蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attribute")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Mana);

	//Max蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxMana);


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
private:
	void SetEffectProperty(const struct FGameplayEffectModCallbackData& Data, FMy_EffectProperties& Props) const;
};

