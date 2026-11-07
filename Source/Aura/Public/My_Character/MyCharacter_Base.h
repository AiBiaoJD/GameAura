// Copyright ABiao
#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "Aura/Public/My_AbilitySystem/Data/My_CharacterClassInfo.h"
#include "Aura/Public/My_AbilitySystem/Debuff/My_DebuffNiagaraComponent.h"
#include "My_Interraction/My_CombatInterface.h"
#include "MyCharacter_Base.generated.h"

class UMy_PassiveNiagaraComponent;
class UNiagaraSystem;


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
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);


	/*
	 * Stun / Burn 状态：动态 GE 无法复制到客户端，
	 * 且 ASC 的 minimal 复制 Tag 带 COND_SkipOwner —— owner 自己收不到。
	 * 所以用复制的 bool 让 owner 客户端也能知道状态（AnimBP 读这个，不读 Tag）
	*/
	UPROPERTY(ReplicatedUsing = OnRep_Stunned, BlueprintReadOnly, Category = "My_Character|Common|Combat")
	bool bIsStunned = false;

	UPROPERTY(ReplicatedUsing = OnRep_Burned, BlueprintReadOnly, Category = "My_Character|Common|Combat")
	bool bIsBurned = false;

	UFUNCTION()
	virtual void OnRep_Stunned();
	virtual void StunTagChanged(const FGameplayTag CallBackTag, int32 NewCount);

	UFUNCTION()
	virtual void OnRep_Burned();
	virtual void BurnTagChanged(const FGameplayTag CallBackTag, int32 NewCount);


	/* 伤害 */
	// 引擎算完径向衰减后会调这里，我们在里面把结果广播出去
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

	/** Combat interface **/
	virtual FVector GetWeaponSockLocation_Implementation(const FGameplayTag& MontageTag) override;
	virtual void Die(const FVector& DeathImpulse) override;
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FMy_TaggedMontage> GetAttackMontage_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FMy_TaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;
	virtual int32 GetMinionCount_Implementation() override;
	virtual void IncreaseMinionCount_Implementation(int32 num) override;
	virtual EMy_CharacterClass GetCharacterClass_Implementation() override;
	virtual FMy_ASCRegisteredSignature& GetOnASCRegistered() override;
	virtual FMy_DeathSignature& GetOnDeath() override;
	virtual FMy_OnDamageSignature& GetOnDamageSignature() override;
	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;
	/** end Combat interface **/
	FMy_ASCRegisteredSignature OnASCRegistered;
	FMy_DeathSignature OnDeath;
	FMy_OnDamageSignature OnDamageDelegate;

	/* 重新应用 Secondary GE，强制 MMC 重算 MaxHealth / MaxMana。
	 * Infinite GE 里的 MMC 只在 ApplyGameplayEffectSpec 那一刻求值一次，
	 * 之后 Level / Vigor / Intelligence 再变它也不会自己重算。
	 * bTopOff 为 true 时顺手把 Health / Mana 补满（升级用，加点不补）。 */
	void RefreshSecondaryAttributes(bool bTopOff);

	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】近战连击列表：Tag → 蒙太奇 + 插槽。玩家和敌人共用。"))
	TArray<FMy_TaggedMontage> AttackMontage;

protected:
	virtual void BeginPlay() override;

	//创建武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】武器骨骼网格。玩家和敌人都用。"))
	TObjectPtr<USkeletalMeshComponent> Weapon;
	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】武器尖端插槽名。近战判定点和焰矢发射点都读它。"))
	FName WeaponTipSockName;

	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】左手插槽，施法动画用。"))
	FName LeftHandSockName;

	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】右手插槽，施法动画用。"))
	FName RightHandSockName;

	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】尾巴插槽，施法动画用。"))
	FName TailSockName;

	bool bDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】基础移动速度。"))
	float BaseWalkSpeed = 600.f;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Attribute", meta = (ToolTip = "【通用】初始化主属性（力量/智力/韧性/活力）。Aura 用 My_GE_PrimaryAttribute_Aura；敌人走 CharacterClassInfo 里对应职业的 GE。"))
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributeEffectClass;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Attribute", meta = (ToolTip = "【通用】初始化次属性（护甲/暴击/抗性/MaxHealth/MaxMana）。"))
	TSubclassOf<UGameplayEffect> DefaultSecondAttributeEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Attribute", meta = (ToolTip = "【通用】把当前 Health/Mana 拉满到 Max。玩家和敌人可以共用同一个资产。"))
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Dissolve", meta = (ToolTip = "【通用】身体死亡溶解材质。"))
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Dissolve", meta = (ToolTip = "【通用】武器死亡溶解材质。"))
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】近战命中时播放的血花特效。"))
	UNiagaraSystem* MeleeBloodEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】死亡音效。"))
	USoundBase* DeathSound;

	/* Minions */
	int32 MinionCount = 0;

	/* Character Class*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Character|Common|Class", meta = (ToolTip = "【通用】角色职业。玩家影响 UI 图标；敌人在 CharacterClassInfo 里决定用哪套属性 GE。"))
	EMy_CharacterClass CharacterClass = EMy_CharacterClass::Warrior;

	/* Debuff*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMy_DebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMy_DebuffNiagaraComponent> StunDebuffComponent;

	/* Passive Niagara*/
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UMy_PassiveNiagaraComponent> HaloOfProtectionPassiveComponent;
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UMy_PassiveNiagaraComponent> LifeSiphonPassiveComponent;
	UPROPERTY(visibleAnywhere)
	TObjectPtr<UMy_PassiveNiagaraComponent> ManaSiphonPassiveComponent;
	UPROPERTY(visibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

private:
	/* 这些变量刚好没被Enemy使用,只是给Aura的
	 * Enemy的设置在UMy_CharacterClassInfo这里DataAsset 
	 */
	UPROPERTY(EditAnywhere, Category = "My_Character|Aura|Abilities", meta = (ToolTip = "【仅玩家 Aura】进关卡时自动授予的技能。敌人走 My_CharacterClassInfo 的 StartupAbilities，不读这里。"))
	TArray<TSubclassOf<UGameplayAbility>> StartupAbility;

	UPROPERTY(EditAnywhere, Category = "My_Character|Aura|Abilities", meta = (ToolTip = "【仅玩家 Aura】开局直授的被动技能。"))
	TArray<TSubclassOf<UGameplayAbility>> StartupPassiveAbility;

	UPROPERTY(EditAnywhere, Category = "My_Character|Common|Combat", meta = (ToolTip = "【通用】受击蒙太奇。敌人必须配；玩家可以留空。"))
	TObjectPtr<UAnimMontage> HitReactMontage;
};
