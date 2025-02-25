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

	//ReplicatedUsing = OnRep_Health,属性更新时执行回调函数
	//血量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	//宏定义
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Health);

	//Max血量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "MaxHealth")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxHealth);


	//蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Mana);

	//Max蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "MaxMana")
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

};

