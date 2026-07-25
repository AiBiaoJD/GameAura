// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "My_AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_OnPlayerStateChanged, int32 /*StateValue*/)

/**
 *
 */
UCLASS()
class AURA_API AMy_AuraPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMy_AuraPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//共有成员函数返回Protect/Private成员变量
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;
	
	FMy_OnPlayerStateChanged OnXPChanged;
	FMy_OnPlayerStateChanged OnLevelChanged;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }	
	
	void SetXP(int32 number);
	void SetLevel(int32 number);
	void AddXP(int32 number);
	void AddToLevel(int32 number);
	

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;

	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP;

	UFUNCTION()
	void OnRep_XP(int32 OldXP);
};
