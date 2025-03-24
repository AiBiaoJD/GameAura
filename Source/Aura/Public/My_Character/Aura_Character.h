// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "My_Character/MyCharacter_Base.h"
#include "Aura_Character.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAura_Character : public AMyCharacter_Base
{
	GENERATED_BODY()
public:
	
	AAura_Character();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	/** Combat interface **/
	virtual int32 GetPlayerLevel() override;
	/** end Combat interface **/
private:

	//初始化HUD和相关ASC委托的绑定
	virtual void My_InitAbilityActorInfo() override;
};
