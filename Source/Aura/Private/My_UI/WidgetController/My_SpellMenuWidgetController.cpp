// Copyright ABiao


#include "My_UI/WidgetController/My_SpellMenuWidgetController.h"

#include "My_AuraGamePlayTags_Singleton.h"


void UMy_SpellMenuWidgetController::BroadcastInitiaValues()
{
	BroadcastAbilityInfo();
	OnPlayerSpellPointChanged.Broadcast(GetAuraPS()->GetSpellPoint());
}

void UMy_SpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->OnAbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag)
	{
		if (AbilityDA)
		{
			FMy_AuraAbilityInfo info = AbilityDA->FindAbilityInfoFromTag(AbilityTags);
			info.StatusTag = StatusTag;
			OnAbilityInfo.Broadcast(info);
		}
	});
	if (GetAuraPS())
	{
		GetAuraPS()->OnSpellPointChanged.AddLambda([this](int32 NewSpellPoint)
		{
			OnPlayerSpellPointChanged.Broadcast(NewSpellPoint);
		});
	}
}

void UMy_SpellMenuWidgetController::SpellGlobeSelecoted(const FGameplayTag& AbilityTag)
{
	const int32 SpellPoint = GetAuraPS()->GetSpellPoint();

	FGameplayTag AbilityStatus;
	const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();

	const bool bTagValid = AbilityTag.IsValid(); //AbilityTag合法
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.My_Abilities_None); //AbilityTag为空
	const FGameplayAbilitySpec* Spec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = Spec != nullptr; //Ability存在

	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = GameplayTags.My_Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusTagFromAbilitySpec(*Spec);
	}

	bool bSpendPointsButtonEnabled = false;
	bool bEquipButtonEnabled = false;
	My_ShouldEnableButtton(AbilityStatus, SpellPoint, bSpendPointsButtonEnabled, bEquipButtonEnabled);

	OnSpellGlobeSelect.Broadcast(bSpendPointsButtonEnabled, bEquipButtonEnabled);
}

void UMy_SpellMenuWidgetController::My_ShouldEnableButtton(FGameplayTag AbilityStatus, const int32 SpellPoint, bool& bSpendPointsButtonEnabled, bool& bEquipButtonEnabled)
{
	const FMy_AuraGameplayTags GameplayTags = FMy_AuraGameplayTags::GetInstance();
	if (AbilityStatus == GameplayTags.My_Abilities_Status_Locked)
	{
		bEquipButtonEnabled = false;
		bSpendPointsButtonEnabled = false;
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Eligible)
	{
		bEquipButtonEnabled = false;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Equipped)
	{
		bEquipButtonEnabled = true;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
	else if (AbilityStatus == GameplayTags.My_Abilities_Status_Unlocked)
	{
		bEquipButtonEnabled = true;
		if (SpellPoint > 0)
		{
			bSpendPointsButtonEnabled = true;
		}
	}
}
