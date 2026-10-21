// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "My_PassiveNiagaraComponent.generated.h"

class UMy_AuraAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API UMy_PassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UMy_PassiveNiagaraComponent();
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;
	void OnPassiveActivate(const FGameplayTag& AbilityTags, bool bActivate);
	// 开局直授的被动不会走 Multicast，这里主动查一次状态兜底
	void ActivateIfEquipped(UMy_AuraAbilitySystemComponent* AuraASC);
};
