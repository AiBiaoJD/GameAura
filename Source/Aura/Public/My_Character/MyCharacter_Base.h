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
	UAttributeSet* GetAttributeSet()const;

protected:
	virtual void BeginPlay() override;

	//创建武器
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	//GAS
	//Aura：实现下面在controler/playerstater中
	//敌人：实现下面在构造函数中
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void My_InitAbilityActorInfo();


	//Effect 初始化相关属性
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributeEffectClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> DefaultSecondAttributeEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	TSubclassOf<UGameplayEffect> InitVitalAttributeEffectClass;

	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GamePlayEffect, float Level) const;
	void InitializeDefaultAttribute() const;

	// Ability初始化相关
	void AddCharacterAbilities();
private:
	UPROPERTY(EditAnywhere, Category = "My_Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;
};
