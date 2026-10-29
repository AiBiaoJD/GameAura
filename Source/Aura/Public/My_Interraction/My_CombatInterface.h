// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Aura/Public/My_AbilitySystem/Data/My_CharacterClassInfo.h"
#include "UObject/Interface.h"
#include "My_CombatInterface.generated.h"

class UNiagaraSystem;

DECLARE_MULTICAST_DELEGATE_OneParam(FMy_ASCRegisteredSignature, UAbilitySystemComponent*)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMy_DeathSignature, AActor*, DeadActor);
DECLARE_MULTICAST_DELEGATE_OneParam(FMy_OnDamageSignature, float);

USTRUCT(BlueprintType)
struct FMy_TaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MontageTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag SocketTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoundBase* ImpactSound = nullptr;
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType)
class UMy_CombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IMy_CombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetWeaponSockLocation(const FGameplayTag& MontageTag);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetWeapon();
	

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void My_UpdateFacingTarget(const FVector& TargetLoc);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die(const FVector& DeathImpulse) = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FMy_TaggedMontage> GetAttackMontage();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FMy_TaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncreaseMinionCount(int32 num);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetPlayerLevel();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EMy_CharacterClass GetCharacterClass();

	virtual FMy_ASCRegisteredSignature& GetOnASCRegistered() = 0;
	virtual FMy_DeathSignature& GetOnDeath() = 0;

	// 伤害广播：TakeDamage 里喊一声，让 ExecCalc 能截获引擎算好的衰减伤害
	virtual FMy_OnDamageSignature& GetOnDamageSignature() = 0;
};
