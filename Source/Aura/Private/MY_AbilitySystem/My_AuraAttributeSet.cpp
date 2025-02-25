// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAttributeSet.h"

#include "Net/UnrealNetwork.h"

UMy_AuraAttributeSet::UMy_AuraAttributeSet()
{

	//�궨��򻯰�GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
	InitHealth(200.0f);
	InitMaxHealth(300.f);
	InitMana(150.f);
	InitMaxMana(150.f);
}

void UMy_AuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate �������ĸ��ƣ������е���OnRep����ʱ��ͬ
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UMy_AuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//�꣬�ȽϾ�ֵ����ֵ����ͬ����OnRep������GetLifetimeReplicatedProps������ͬҲ����
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Health, OldHealth);

}
void UMy_AuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Health, OldMaxHealth);
}

void UMy_AuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Mana, OldMana);
}

void UMy_AuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, MaxMana, OldMaxMana);
}
