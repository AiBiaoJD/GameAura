// Copyright ABiao
#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "My_Interraction/My_CombatInterface.h"
#include "MyCharacter_Base.generated.h"


UCLASS(ABSTRACT)
class AURA_API AMyCharacter_Base : public ACharacter, public IAbilitySystemInterface, public
                                   IMy_CombatInterface
{
	GENERATED_BODY()

public:
	AMyCharacter_Base();

	//角色ASC通过这个函数从PlayerState获取
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const;

	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	/** Combat interface **/
	virtual FVector GetWeaponSockLocation_Implementation() override;
	virtual void Die() override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FMy_TaggedMontage> GetAttackMontage_Implementation() override;
	/** end Combat interface **/

	UPROPERTY(EditAnywhere, Category = "My_Combat")
	TArray<FMy_TaggedMontage> AttackMontage;
protected:
	virtual void BeginPlay() override;

	//创建武器
	UPROPERTY(EditAnywhere, Category = "My_Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "My_Combat")
	FName WeaponTipSockName;

	bool bDead = false;

	/*
	 * GAS
	 * Aura：实现下面在controler/playerstater中
	 * 敌人：实现下面在构造函数中
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	//Effect 初始化相关属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributeEffectClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondAttributeEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> InitVitalAttributeEffectClass;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GamePlayEffect, float Level) const;
	virtual void My_InitAbilityActorInfo();
	virtual void InitializeDefaultAttribute() const;

	// Ability初始化相关
	void AddCharacterAbilities();

	/* Dissolve Effect*/

	void Dissolve();

	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(UMaterialInstanceDynamic* MaterialInstanceDynamic);
	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeline(UMaterialInstanceDynamic* MaterialInstanceDynamic);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere, Category = "My_Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;

	UPROPERTY(EditAnywhere, Category = "My_Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
