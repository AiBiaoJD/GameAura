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
	1.������Щ������Ҫ�������ϸ���
	2.ÿ�����������ض��ĸ��������͹���
	3.ͨ����ȷ������Ҫ���Ƶ����ԣ������Ż��������ʹ��
	*/
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//ReplicatedUsing = OnRep_Health,���Ը���ʱִ�лص�����
	//Ѫ��
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health")
	FGameplayAttributeData Health;
	//�궨��
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Health);

	//MaxѪ��
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "MaxHealth")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, MaxHealth);


	//����
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Mana")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UMy_AuraAttributeSet, Mana);

	//Max����
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

