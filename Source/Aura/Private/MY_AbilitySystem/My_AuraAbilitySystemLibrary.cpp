// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "My_Controler/My_AuraPlayerState.h"
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
