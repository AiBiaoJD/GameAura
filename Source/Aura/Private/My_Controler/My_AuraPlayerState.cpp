// Copyright ABiao


#include "My_Controler/My_AuraPlayerState.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AMy_AuraPlayerState::AMy_AuraPlayerState()
{
	NetUpdateFrequency = 100.0f;

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMy_AuraAbilitySystemComponent
	>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UMy_AuraAttributeSet>("AttributeSet");
}

void AMy_AuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMy_AuraPlayerState, Level);
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
}
