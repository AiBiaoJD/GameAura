// Copyright ABiao

#include "My_Character/Enemy_Characte.h"
#include "DrawDebugHelpers.h"
void AEnemy_Characte::HighlightActor()
{
	//// 获取角色的包围盒
	FVector Origin, Extent;
	GetActorBounds(true, Origin, Extent);
	// 打印包围盒信息
	UE_LOG(LogTemp, Warning, TEXT("Origin: %s, Extent: %s"), *Origin.ToString(), *Extent.ToString());
	// 绘制包围盒
	DrawDebugSphere(GetWorld(), Origin, 50.0f, 12, FColor::Green, false, 5.0f, 0, 2.0f);
	UE_LOG(LogTemp, Warning, TEXT("hit"));
}

void AEnemy_Characte::UnHighlightActor()
{
	FlushPersistentDebugLines(GetWorld());
	UE_LOG(LogTemp, Warning, TEXT("no hit"));

}
