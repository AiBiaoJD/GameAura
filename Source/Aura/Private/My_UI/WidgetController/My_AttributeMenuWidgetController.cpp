// Copyright ABiao


#include "My_UI/WidgetController/My_AttributeMenuWidgetController.h"

#include "MY_AbilitySystem/My_AuraAbilitySystemComponent.h"
#include "MY_AbilitySystem/My_AuraAttributeSet.h"
#include "My_Controler/My_AuraPlayerState.h"


void UMy_AttributeMenuWidgetController::BroadcastInitiaValues()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);

	check(AttributeDA);

	for (auto& Pair : AS->M_TagsToAttribute)
	{
		BroadAttributeInfo(Pair.Key, Pair.Value());
	}

	AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
	if (MyPS)
	{
		OnPlayerAttributeChanged.Broadcast(MyPS->GetAttributePoint());
	}
}

void UMy_AttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UMy_AuraAttributeSet* AS = CastChecked<UMy_AuraAttributeSet>(AttributeSet);
	check(AttributeDA);

	for (auto& Pair : AS->M_TagsToAttribute)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda([this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadAttributeInfo(Pair.Key, Pair.Value());
		});
	}

	AMy_AuraPlayerState* MyPS = Cast<AMy_AuraPlayerState>(PlayerState);
	if (MyPS)
	{
		MyPS->OnAttributePointChanged.AddLambda([this](int32 NewAttributePoint)
		{
			OnPlayerAttributeChanged.Broadcast(NewAttributePoint);
		});
	}
}

void UMy_AttributeMenuWidgetController::BroadAttributeInfo(const FGameplayTag& Tag, const FGameplayAttribute& Attribute)
{
	FMy_AuraAttributeInfo Info = AttributeDA->FindAttributeinfoFormTag(Tag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	OnAttributeInfo.Broadcast(Info);
}

void UMy_AttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UMy_AuraAbilitySystemComponent* AuraASC = CastChecked<UMy_AuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraASC->UpgradeAttribute(AttributeTag);
}
