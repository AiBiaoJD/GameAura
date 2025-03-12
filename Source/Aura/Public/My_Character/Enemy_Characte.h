// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "My_Character/MyCharacter_Base.h"
#include "My_Interraction/My_Enemy_Interface.h"
#include "Enemy_Characte.generated.h"

/**
 *
 */
UCLASS()
class AURA_API AEnemy_Characte : public AMyCharacter_Base, public  IMy_Enemy_Interface
{
	GENERATED_BODY()
public:
	/** Enemy interface **/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy interface **/

	AEnemy_Characte();
protected:
	virtual  void BeginPlay() override;

	virtual  void My_InitAbilityActorInfo() override;


	//只关心检查敌人Level在服务器上,不需要Replied
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Default")
	int32 Level = 1;
};
