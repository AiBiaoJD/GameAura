// Copyright ABiao


#include "My_AI/My_BTService_FindNearestPlayer.h"
#include "AIController.h"

void UMy_BTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	GEngine->AddOnScreenDebugMessage(1,1.f,FColor::Red,*AIOwner->GetName());
	
	GEngine->AddOnScreenDebugMessage(2,1.f,FColor::Green,*ActorOwner->GetName());
}
