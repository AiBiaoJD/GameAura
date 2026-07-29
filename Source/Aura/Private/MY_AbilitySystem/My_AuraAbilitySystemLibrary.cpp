// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"

#include "My_AuraAbilityTypes.h"
#include "Kismet/GameplayStatics.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "MY_GameMode/MyGameModeBase.h"
#include "My_Interraction/My_CombatInterface.h"
#include "My_UI/HUD/My_AuraHUD.h"

UMy_OverlayWidgetController* UMy_AuraAbilitySystemLibrary::My_GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMy_AuraHUD* AuraHUD = Cast<AMy_AuraHUD>(PC->GetHUD()))
		{
			AMy_AuraPlayerState* PS = PC->GetPlayerState<AMy_AuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FMY_WidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UMy_AttributeMenuWidgetController* UMy_AuraAbilitySystemLibrary::My_GetMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMy_AuraHUD* AuraHUD = Cast<AMy_AuraHUD>(PC->GetHUD()))
		{
			AMy_AuraPlayerState* PS = PC->GetPlayerState<AMy_AuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();

			const FMY_WidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return AuraHUD->GetMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UMy_AuraAbilitySystemLibrary::GiveDefaultAttributes(const UObject* WorldContextObject,
                                                         EMy_CharacterClass CharacterType, float level,
                                                         UAbilitySystemComponent* ASC)
{
	UMy_CharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	FMy_CharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterType);

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(ASC->GetAvatarActor());

	const FGameplayEffectSpecHandle PrimatySpecHandle = ASC->MakeOutgoingSpec(
		ClassDefaultInfo.PrimaryAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimatySpecHandle.Data.Get());

	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(
		ClassInfo->SecondaryAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(
		ClassInfo->VitalAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}

void UMy_AuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, EMy_CharacterClass CharacterType)
{
	UMy_CharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr) return;

	//共有能力
	for (auto Ability : ClassInfo->CommonAbility)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		ASC->GiveAbility(AbilitySpec);
	}

	//特定能力
	const FMy_CharacterClassDefaultInfo& DefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterType);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(ASC->GetAvatarActor());
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
		ASC->GiveAbility(AbilitySpec);
	}
}

int32 UMy_AuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, EMy_CharacterClass CharacterType, int32 level)
{
	UMy_CharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr) return 0;
	FMy_CharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterType);
	float Xp = ClassDefaultInfo.XPReward.GetValueAtLevel(level);
	return static_cast<int32>(Xp);
}

UMy_CharacterClassInfo* UMy_AuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AMyGameModeBase* AuraGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode) return nullptr;
	return AuraGameMode->CharacterClassInfo;
}

bool UMy_AuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMY_AuraGamePlayEffectContext* AuraGamePlayEffectContext = static_cast<const FMY_AuraGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGamePlayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UMy_AuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FMY_AuraGamePlayEffectContext* AuraGamePlayEffectContext = static_cast<const FMY_AuraGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		return AuraGamePlayEffectContext->IsCriticalHit();
	}
	return false;
}

void UMy_AuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool IsBlocked)
{
	if (FMY_AuraGamePlayEffectContext* AuraGamePlayEffectContext = static_cast<FMY_AuraGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGamePlayEffectContext->SetIsBlockedHit(IsBlocked);
	}
}

void UMy_AuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool IsCritical)
{
	if (FMY_AuraGamePlayEffectContext* AuraGamePlayEffectContext = static_cast<FMY_AuraGamePlayEffectContext*>(EffectContextHandle.Get()))
	{
		AuraGamePlayEffectContext->SetIsCriticalHit(IsCritical);
	}
}

void UMy_AuraAbilitySystemLibrary::GetLivePlayersWithRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverLappingActors, const TArray<AActor*>& ActorToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorToIgnore);

	TArray<FOverlapResult> Overlaps;
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UMy_CombatInterface>() && !IMy_CombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				AActor* Avatar = IMy_CombatInterface::Execute_GetAvatar(Overlap.GetActor());
				UE_LOG(LogTemp, Warning, TEXT("------------1------------"));

				// 关键：添加去重检查
				if (Avatar && !OutOverLappingActors.Contains(Avatar))
				{
					OutOverLappingActors.Add(Avatar);
					UE_LOG(LogTemp, Warning, TEXT("Add new Character: %s"), *Avatar->GetName());
				}
				else if (Avatar)
				{
					UE_LOG(LogTemp, Warning, TEXT("Ignore same Character: %s"), *Avatar->GetName());
				}

				UE_LOG(LogTemp, Warning, TEXT("------------2------------"));

			}
		}
	}
}

bool UMy_AuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool BothArePlayer = FirstActor->ActorHasTag("Player") && SecondActor->ActorHasTag("Player");
	const bool BothAreEnemy = FirstActor->ActorHasTag("Enemy") && SecondActor->ActorHasTag("Enemy");
	const bool Friends = BothArePlayer || BothAreEnemy;
	return !Friends;
}




