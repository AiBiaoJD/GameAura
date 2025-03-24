// Copyright ABiao


#include "MY_AbilitySystem/My_AuraAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "My_Character/Enemy_Characte.h"
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

UMy_EnemyHealthWidgetController* UMy_AuraAbilitySystemLibrary::My_GetEnemyHealthWidgetController(const UObject* WorldContextObject, AEnemy_Characte* Enemy)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AMy_AuraHUD* AuraHUD = Cast<AMy_AuraHUD>(PC->GetHUD()))
		{
			check(Enemy);
			UAbilitySystemComponent* ASC = Enemy->GetAbilitySystemComponent();
			UAttributeSet* AS = Enemy->GetAttributeSet();

			const FMY_WidgetControllerParams WidgetControllerParams(nullptr, nullptr, ASC, AS);
			return AuraHUD->GetEnemyHealthWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}




