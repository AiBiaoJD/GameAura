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
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;

	AEnemy_Characte();
};
