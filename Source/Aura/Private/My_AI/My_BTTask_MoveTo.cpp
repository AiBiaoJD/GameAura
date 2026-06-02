// Copyright ABiao

#include "My_AI/My_BTTask_MoveTo.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/BehaviorTree.h"

UMy_BTTask_MoveTo::UMy_BTTask_MoveTo()
{
	AcceptableRadiusKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UMy_BTTask_MoveTo, AcceptableRadiusKey));
}

void UMy_BTTask_MoveTo::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	// 从黑板资产解析 AcceptableRadiusKey：把编辑器选的 Key 名字转成运行时 KeyID
	AcceptableRadiusKey.ResolveSelectedKey(*Asset.BlackboardAsset);
}

EBTNodeResult::Type UMy_BTTask_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 用 IsSet() 判断：Key 是否已成功解析（SelectedKeyID 有效）
	if (AcceptableRadiusKey.IsSet())
	{
		const float BBValue = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(AcceptableRadiusKey.SelectedKeyName);
		AcceptableRadius = BBValue;
		UE_LOG(LogTemp, Warning, TEXT("[My_BTTask_MoveTo] AcceptableRadius from BB: %f"), BBValue);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[My_BTTask_MoveTo] AcceptableRadiusKey 没设置！用了默认值: %f"), AcceptableRadius);
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
