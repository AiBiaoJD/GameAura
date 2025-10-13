// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "My_AuraAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class AURA_API AMy_AuraAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMy_AuraAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
};
