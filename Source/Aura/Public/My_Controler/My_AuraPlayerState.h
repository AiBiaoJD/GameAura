// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "My_AuraPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_PlayerStateChangedSignature, int32 /*StateValue*/)

class UMy_LevelUpInfo;
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
	
	FMy_PlayerStateChangedSignature OnXPChanged;
	FMy_PlayerStateChangedSignature OnLevelChanged;
	FMy_PlayerStateChangedSignature OnAttributePointChanged;
	FMy_PlayerStateChangedSignature OnSpellPointChanged;

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE int32 GetXP() const { return XP; }
	FORCEINLINE int32 GetAttributePoint() const { return AttributePoint; }
	FORCEINLINE int32 GetSpellPoint() const { return SpellPoint; }

	
	void SetXP(int32 number);
	void SetLevel(int32 number);
	void SetAttributePoint(int32 number);
	void SetSpellPoint(int32 number);
	
	void AddXP(int32 number);
	void AddToLevel(int32 number);
	void AddAttributePoint(int32 number);
	void AddSpellPoint(int32 number);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMy_LevelUpInfo> LevelUpInfo;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;


private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP)
	int32 XP;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_AttributePoint)
	int32 AttributePoint;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_SpellPoint)
	int32 SpellPoint;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	UFUNCTION()
	void OnRep_XP(int32 OldXP);
	UFUNCTION()
	void OnRep_AttributePoint(int32 OldAttributePoint);
	UFUNCTION()
	void OnRep_SpellPoint(int32 OldSpellPoint);
};

