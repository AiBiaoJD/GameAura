// Copyright ABiao


#include "My_AI/My_BTService_FindNearestPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UMy_BTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* OwningPawn = AIOwner ? AIOwner->GetPawn() : nullptr;

	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> ActorWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, ActorWithTag);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* Actor : ActorWithTag)
	{
		if (IsValid(Actor) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(Actor);
			if (Distance < ClosestDistance && Distance <= SearchRadius)
			{
				ClosestDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector,ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToTargetSelector,ClosestDistance);
}
