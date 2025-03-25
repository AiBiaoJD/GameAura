// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "My_Controler/My_AuraPlayerState.h"
#include "MY_GameMode/MyGameModeBase.h"
#include "My_UI/HUD/My_AuraHUD.h"

UMy_OverlayWidgetController* UMy_AuraAbilitySystemLibrary::My_GetOverlayWidgetController(const UObject* WorldContextObject)
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

void UMy_AuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, EMy_CharacterClass CharacterType, float level, UAbilitySystemComponent* ASC)
{
	AMyGameModeBase* AuraGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!AuraGameMode) return;

	UMy_CharacterClassInfo* ClassInfo = AuraGameMode->CharacterClassInfo;
	FMy_CharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterType);

	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(ASC->GetAvatarActor());

	const FGameplayEffectSpecHandle PrimatySpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimatySpecHandle.Data.Get());

	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, level, ContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}

