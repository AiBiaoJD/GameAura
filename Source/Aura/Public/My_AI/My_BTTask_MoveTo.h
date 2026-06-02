// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "My_BTTask_MoveTo.generated.h"

/**
 *  扩展原生MoveTo，支持从黑板读取AcceptableRadius
 */
UCLASS()
class AURA_API UMy_BTTask_MoveTo : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:
	UMy_BTTask_MoveTo();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AcceptableRadiusKey;
};
