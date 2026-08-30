// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "Aura/AuraLogChannels.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"
#include "MY_AbilitySystem/Data/My_AbilityInfo.h"
#include "My_Interraction/My_PlayerInterface.h"

UMy_AuraAbilitySystemComponent::UMy_AuraAbilitySystemComponent()
{
	SetIsReplicated(true);
}

//在Character_Base的SetAbilityActorInfo()函数中调用
void UMy_AuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//-------一旦Actorinfo设置好,就可以对ASC的委托进行绑定-----------------
	// 该委托只在服务器调用,客户端不使用,因此使用RPC方法解决问题
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UMy_AuraAbilitySystemComponent::ClientEffectApplied);
}

//把"服务器上施加了 GE"这件事通知给客户端，让 UI 显示效果消息（比如拾取药水、获得增益）
void UMy_AuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                        const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	//使用委托进行TagContainer的广播,这样WidgetController就可以接受到
	//这样My_ASC不知道WidgetController,而WidgetController知道ASC
	EffectAssetTags.Broadcast(TagContainer);
}

/*
 * 在角色初始化时,给能力添加Tag
 */
void UMy_AuraAbilitySystemComponent::AddCharacterAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbility)
{
	for (auto& Ability : StartupAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);

		if (const UMy_AuraGameplayAbilityBase* AuraAbility = Cast<UMy_AuraGameplayAbilityBase>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartUpInputTag);
			AbilitySpec.DynamicAbilityTags.AddTag(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
	}

	//处理技能UI显示的旗子标记
	bStartupAbilityGiven = true;
	/* 角色添加能力的时候，可以给这个能力设置UI，和AbilityInfo对比*/
	OnAbilityGiven.Broadcast();
}

void UMy_AuraAbilitySystemComponent::AddCharacterPassiveAbilitiesFromASC(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbility)
{
	for (auto& Ability : StartupPassiveAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

//处理客户端不显示Ability的UI问题
void UMy_AuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if (!bStartupAbilityGiven)
	{
		bStartupAbilityGiven = true;
		OnAbilityGiven.Broadcast();
	}
}


/*
 * 当PlayerController 按下/放开 按键会激活下面的函数
 * 根据inputTag激活对应的能力
 */
void UMy_AuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UMy_AuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (!InputTag.IsValid()) return;

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UMy_AuraAbilitySystemComponent::ForEachAbility(const FMy_ForEachAbilitySignature& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAura, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetAbilityTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_Abilities")))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetInputTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_InputTag")))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UMy_AuraAbilitySystemComponent::GetStatusTagFromAbilitySpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (FGameplayTag Tag : AbilitySpec.DynamicAbilityTags)
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("My_Abilities.Status")))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

void UMy_AuraAbilitySystemComponent::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (GetAvatarActor()->Implements<UMy_PlayerInterface>())
	{
		//有属性点
		if (IMy_PlayerInterface::Execute_GetAttributePointFormPlayerState(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag);
		}
	}
}

void UMy_AuraAbilitySystemComponent::ServerUpgradeAttribute_Implementation(const FGameplayTag AttributeTag)
{
	if (IMy_PlayerInterface::Execute_GetAttributePointFormPlayerState(GetAvatarActor()) <= 0) return;
	FGameplayEventData EventData;
	EventData.EventTag = AttributeTag;
	EventData.EventMagnitude = 1.f;
	//使用SendEvent的方式修改Attribute,因为PassiveAbility可以监听AttributeTag的Event
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, EventData);
	//属性点-1
	IMy_PlayerInterface::Execute_AddToAttributePoint(GetAvatarActor(), -1);
}

void UMy_AuraAbilitySystemComponent::UpdateAbilityStatuses(int32 Level)
{
	UMy_AbilityInfo* AbilityInfo = UMy_AuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityInfo) return;
	for (const FMy_AuraAbilityInfo& info : AbilityInfo->AbilityInformation)
	{
		if (!info.AbilityTag.IsValid() || !info.AbilityClass || Level < info.LevelUpRequirement) continue;
		if (GetSpecFromAbilityTag(info.AbilityTag) == nullptr)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(info.AbilityClass, 1);
			AbilitySpec.DynamicAbilityTags.AddTag(FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			ClientUpdateAbilityStatus(info.AbilityTag, FMy_AuraGameplayTags::GetInstance().My_Abilities_Status_Eligible);
		}
	}
}

FGameplayAbilitySpec* UMy_AuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		for (const FGameplayTag& Tag : Spec.Ability.Get()->AbilityTags)
		{
			if (Tag.MatchesTag(AbilityTag))
			{
				return &Spec;
			}
		}
	}
	return nullptr;
}
void UMy_AuraAbilitySystemComponent::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTags, const FGameplayTag& StatusTag)
{
	OnAbilityStatusChanged.Broadcast(AbilityTags, StatusTag);
}
