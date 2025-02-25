// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAttributeSet.h"

#include "Net/UnrealNetwork.h"

UMy_AuraAttributeSet::UMy_AuraAttributeSet()
{

	//宏定义简化版GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
	InitHealth(200.0f);
	InitMaxHealth(300.f);
	InitMana(150.f);
	InitMaxMana(150.f);
}

void UMy_AuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate 无条件的复制，总是有调用OnRep，即时相同
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UMy_AuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//宏，比较旧值和新值，不同调用OnRep，上面GetLifetimeReplicatedProps改了相同也调用
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
