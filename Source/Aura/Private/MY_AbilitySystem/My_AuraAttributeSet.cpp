// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "My_AuraGamePlayTags_Singleton.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "My_Controler/My_Aura_Controller.h"
#include "My_Interraction/My_CombatInterface.h"
#include "Net/UnrealNetwork.h"

UMy_AuraAttributeSet::UMy_AuraAttributeSet()
{
	const FMy_AuraGameplayTags& GameplayTags = FMy_AuraGameplayTags::GetInstance();

	// 添加 Primary Attributes
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Primary_Strength, GetStrengthAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Primary_Resilience, GetResilienceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Primary_VIgor, GetVIgorAttribute);

	// 添加 Secondary Attributes
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_Armor, GetArmorAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_MaxMana, GetMaxManaAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_BlockChance, GetBlockChanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_Resistance_Fire, GetFireResistanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_Resistance_Lighting, GetLightingResistanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_Resistance_Arcane, GetArcaneResistanceAttribute);
	M_TagsToAttribute.Add(GameplayTags.My_Attribute_Secondary_Resistance_Physical, GetPhysicalResistanceAttribute);
}

//服务器通知客户端各种ATTribute的更新情况
void UMy_AuraAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Replicate 无条件的复制，总是有调用OnRep，即时相同
	//---- Primary ATTribute -----
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, VIgor, COND_None, REPNOTIFY_Always);


	//---- Vital ATTribute -----
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);


	//---- Second Attribute -----
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, LightingResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);

	//---- Third Attribute -----
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMy_AuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
}

//在ATTribute修改之前进行相关处理，主要避免Attribute里面的数据非法，属于Attribute核心逻辑
//GetGameplayAttributeValueChangeDelegate与这个不同，这个是在获取委托，主要处理UI更新和音效
void UMy_AuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}

	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMana());
	}
}

void UMy_AuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	//好像对于持续的Effect,PostAttributeChange这里也限制不了
}

//在 GameplayEffect 执行完毕后调用
void UMy_AuraAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FMy_EffectProperties Props;
	SetEffectProperty(Data, Props);


	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}

	/*
	 * MetaAttribute部分
	 */
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			UE_LOG(LogTemp, Warning, TEXT("changed Health on %s, Health %f"), *Props.TargetAvatarActor->GetName(), GetHealth());

			const bool bFatal = NewHealth <= 0.f;
			if (bFatal)
			{
				IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(Props.TargetAvatarActor);
				if (CombatInterface)
				{
					CombatInterface->Die();
				}
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FMy_AuraGameplayTags::GetInstance().My_EffectGranted_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}

			const bool bBlock = UMy_AuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCritical = UMy_AuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowDamageText(Props, LocalIncomingDamage, bBlock, bCritical);
		}
	}
}

void UMy_AuraAttributeSet::ShowDamageText(const FMy_EffectProperties& Props, float Damage, bool IsBlockedHit, bool IsCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Isblockhit: %d"), IsBlockedHit);
		UE_LOG(LogTemp, Warning, TEXT("isCriticalHit: %d"), IsCriticalHit);
		if (AMy_Aura_Controller* PC = Cast<AMy_Aura_Controller>(UGameplayStatics::GetPlayerController(Props.SourceCharacter, 0)))
		{
			PC->ClientShowDamageNum(Damage, Props.TargetCharacter, IsBlockedHit, IsCriticalHit);
		}
	}
}

void UMy_AuraAttributeSet::SetEffectProperty(const struct FGameplayEffectModCallbackData& Data, FMy_EffectProperties& Props) const
{
	//Source是造成Effect的来源, Target是Effect的对象(是这个ATTributeSet的拥有者)
	//Instigator是发起者为PlayerState(ASC在上面)
	//Effectcurser是原因为AuraCharacter

	//1.Source部分
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
		//避免PlayerController.Get()是空
		if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if (Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	//2.Target部分
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();

		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}


void UMy_AuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	//宏，比较旧值和新值，不同调用OnRep，上面GetLifetimeReplicatedProps改了相同也调用
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Health, OldHealth);
}

void UMy_AuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UMy_AuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Mana, OldMana);
}

void UMy_AuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, MaxMana, OldMaxMana);
}

void UMy_AuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Strength, OldStrength);
}

void UMy_AuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Intelligence, OldIntelligence);
}

void UMy_AuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Resilience, OldResilience);
}

void UMy_AuraAttributeSet::OnRep_VIgor(const FGameplayAttributeData& OldVIgor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, VIgor, OldVIgor);
}

void UMy_AuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, Armor, OldArmor);
}

void UMy_AuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UMy_AuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, BlockChance, OldBlockChance);
}

void UMy_AuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UMy_AuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UMy_AuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UMy_AuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UMy_AuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UMy_AuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, FireResistance, OldFireResistance);
}

void UMy_AuraAttributeSet::OnRep_LightingResistance(const FGameplayAttributeData& OldLightingResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, LightingResistance, OldLightingResistance);
}

void UMy_AuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UMy_AuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMy_AuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}
