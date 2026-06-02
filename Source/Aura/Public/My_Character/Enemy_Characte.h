// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MY_AbilitySystem/Data/My_CharacterClassInfo.h"
#include "My_Character/MyCharacter_Base.h"
#include "My_Interraction/My_Enemy_Interface.h"
#include "My_UI/WidgetController/My_OverlayWidgetController.h"
#include "Enemy_Characte.generated.h"

class UBehaviorTree;
class AMy_AuraAIController;

/**
 *
 */
UCLASS()
class AURA_API AEnemy_Characte : public AMyCharacter_Base, public IMy_Enemy_Interface
{
	GENERATED_BODY()

public:
	AEnemy_Characte();
	virtual void PossessedBy(AController* NewController) override;

	/** Enemy interface **/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy interface **/

	/** Combat interface **/
	virtual int32 GetPlayerLevel() override;
	virtual void Die() override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() override;
	/** end Combat interface **/


	UPROPERTY(BlueprintAssignable)
	FMy_OnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FMy_OnAttributeChangedSignature OnMaxHealthChanged;

	void HitReatTagChanged(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category = "My_Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Combat")
	float BaseWalkSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Combat")
	float LifeSpan = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "My_Combat")
	float CombatDistance = 100.f;

protected:
	virtual void BeginPlay() override;
	virtual void My_InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttribute() const;


	//只关心检查敌人Level在服务器上,不需要Replied
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Default")
	EMy_CharacterClass CharacterClass;


	/*
	 * 敌人的血量UI 和 血量WidgetController设置
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;


	/*
	 * AI
	 */
	UPROPERTY(EditAnywhere, Category = "My_AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AMy_AuraAIController> AuraAIController;

	UPROPERTY(BlueprintReadWrite, Category = "My_Combat")
	TObjectPtr<AActor> CombatTarget;
};
