// Copyright ABiao


#include "My_Controler/My_AuraPlayerState.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"

AMy_AuraPlayerState::AMy_AuraPlayerState()
{
	NetUpdateFrequency = 100.0f;

	//GAS
	AbilitySystemComponent = CreateDefaultSubobject<UMy_AuraAbilitySystemComponent
	>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UMy_AuraAttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AMy_AuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAttributeSet* AMy_AuraPlayerState::GetAttributeSet() const
{
	return AttributeSet;
}
