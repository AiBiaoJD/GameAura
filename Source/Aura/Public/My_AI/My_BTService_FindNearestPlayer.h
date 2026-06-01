// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "My_BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_BTService_FindNearestPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector TargetToFollowSelector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FBlackboardKeySelector DistanceToTargetSelector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float SearchRadius = 1500.f;
};
