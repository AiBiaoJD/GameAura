// Copyright ABiao

#include "My_Controler/My_AuraPlayerState.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AMy_AuraPlayerState::AMy_AuraPlayerState()
{
	NetUpdateFrequency = 100.0f;

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMy_AuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AttributeSet = CreateDefaultSubobject<UMy_AuraAttributeSet>("AttributeSet");
}

void AMy_AuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMy_AuraPlayerState, Level);
	DOREPLIFETIME(AMy_AuraPlayerState, XP);
	DOREPLIFETIME(AMy_AuraPlayerState, AttributePoint);
	DOREPLIFETIME(AMy_AuraPlayerState, SpellPoint);
}

UAbilitySystemComponent* AMy_AuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AMy_AuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}

void AMy_AuraPlayerState::OnRep_Level(int32 OldLevel)
{
	OnLevelChanged.Broadcast(Level);
}

void AMy_AuraPlayerState::OnRep_XP(int32 OldXP)
{
	OnXPChanged.Broadcast(XP);
}

void AMy_AuraPlayerState::OnRep_AttributePoint(int32 OldAttributePoint)
{
	OnAttributePointChanged.Broadcast(AttributePoint);
}

void AMy_AuraPlayerState::OnRep_SpellPoint(int32 OldSpellPoint)
{
	OnSpellPointChanged.Broadcast(SpellPoint);
}

void AMy_AuraPlayerState::SetXP(int32 number)
{
	XP = number;
	OnXPChanged.Broadcast(XP);
}

void AMy_AuraPlayerState::SetLevel(int32 number)
{
	Level = number;
	OnLevelChanged.Broadcast(Level);
}


void AMy_AuraPlayerState::SetAttributePoint(int32 number)
{
	AttributePoint = number;
	OnAttributePointChanged.Broadcast(AttributePoint);
}

void AMy_AuraPlayerState::SetSpellPoint(int32 number)
{
	SpellPoint = number;
	OnSpellPointChanged.Broadcast(SpellPoint);
}

void AMy_AuraPlayerState::AddXP(int32 number)
{
	XP += number;
	OnXPChanged.Broadcast(XP);
}

void AMy_AuraPlayerState::AddToLevel(int32 number)
{
	Level += number;
	OnLevelChanged.Broadcast(Level);
}
void AMy_AuraPlayerState::AddAttributePoint(int32 number)
{
	AttributePoint += number;
	OnAttributePointChanged.Broadcast(AttributePoint);
}

void AMy_AuraPlayerState::AddSpellPoint(int32 number)
{
	SpellPoint += number;
	OnSpellPointChanged.Broadcast(SpellPoint);
}

