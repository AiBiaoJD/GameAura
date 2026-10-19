// Copyright ABiao


#include "MY_AbilitySystem/Passive/My_PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "My_Interraction/My_CombatInterface.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

UMy_PassiveNiagaraComponent::UMy_PassiveNiagaraComponent()
{
	bAutoActivate = false;
}

void UMy_PassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	// 这里处理和DebuffComponent一样
	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(GetOwner());
	UMy_AuraAbilitySystemComponent* AuraASC = Cast<UMy_AuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()));
	if (AuraASC)
	{
		AuraASC->OnActivatePassiveAbility.AddUObject(this, &UMy_PassiveNiagaraComponent::OnPassiveActivate);
		ActivateIfEquipped(AuraASC);
	}
	else if (CombatInterface)
	{
		CombatInterface->GetOnASCRegistered().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			if (UMy_AuraAbilitySystemComponent* InAuraASC = Cast<UMy_AuraAbilitySystemComponent>(InASC))
			{
				InAuraASC->OnActivatePassiveAbility.AddUObject(this, &UMy_PassiveNiagaraComponent::OnPassiveActivate);
				ActivateIfEquipped(InAuraASC);
			}
		});
	}
}

/* 开局直授的被动（AddCharacterPassiveAbilitiesFromASC → GiveAbilityAndActivateOnce）
 * 在 ServerEquipAbility 第⑥步会因为 IsActive() 已为 true 而被整个跳过，
 * 永远等不到 MulticastActivatePassiveEffect。所以这里按"当前状态"补一次。
 * 事件管实时变化，状态快照管迟到者 —— 两者都要有。 */
void UMy_PassiveNiagaraComponent::ActivateIfEquipped(UMy_AuraAbilitySystemComponent* AuraASC)
{
	if (AuraASC == nullptr || !AuraASC->bStartupAbilityGiven) return;

	const FGameplayTag StatusTag = AuraASC->GetStatusTagFromAbilityTag(PassiveSpellTag);
	if (StatusTag.MatchesTagExact(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Equipped))
	{
		Activate();
	}
}

void UMy_PassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTags, bool bActivate)
{
	if (AbilityTags.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else
		{
			Deactivate();
		}
	}
}
