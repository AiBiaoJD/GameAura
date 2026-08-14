// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "My_Character/MyCharacter_Base.h"
#include "My_Interraction/My_PlayerInterface.h"
#include "Aura_Character.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAura_Character : public AMyCharacter_Base, public IMy_PlayerInterface
{
	GENERATED_BODY()

public:
	AAura_Character();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Player interface **/
	virtual int32 FindLevelForXP_Implementation(int32 InXP) override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 GetAttributePointReward_Implementation(int32 level) const override;
	virtual int32 GetAttributePointFormPlayerState_Implementation() const override;
	virtual int32 GetSpellPointReward_Implementation(int32 level) const override;
	virtual int32 GetSpellPointFormPlayerState_Implementation() const override;
	virtual void AddToXP_Implementation(int32 InXp) override;
	virtual void AddToPlayerLevel_Implementation(int32 InPlayerLevel) override;
	virtual	void AddToAttributePoint_Implementation(int32 InAttributePoint) override;
	virtual	void AddToSpellPoint_Implementation(int32 InSpellPoint) override;
	virtual void LevelUp_Implementation() override;
	/** end Player interface **/

	/** Combat interface **/
	virtual int32 GetPlayerLevel_Implementation() override;
	/** end Combat interface **/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;
	
	//初始化HUD和相关ASC委托的绑定
	virtual void My_InitAbilityActorInfo() override;

	//MultRPC方便所有客户端看见其他客户端的特效
	UFUNCTION(NetMulticast, reliable)
	void MulticastLevelUpNiagaraFun() const;
};
