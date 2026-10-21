// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "My_WaitCoolDownChange.generated.h"

class UAbilitySystemComponent;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_CoolDownChangeSignature, float, TimeRemaining);
/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UMy_WaitCoolDownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FMy_CoolDownChangeSignature CoolDownStart;
	UPROPERTY(BlueprintAssignable)
	FMy_CoolDownChangeSignature CoolDownEnd;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly = "true"))
	static UMy_WaitCoolDownChange* WaitCoolDownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCoolDownTag);

	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CoolDownTag;

	void CoolDownTagChanged(const FGameplayTag InCoolDownTag, int32 NewCount);
	void OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
};
