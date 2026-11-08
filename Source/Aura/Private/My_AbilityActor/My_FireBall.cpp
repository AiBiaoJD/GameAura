// Copyright ABiao


#include "My_AbilityActor/My_FireBall.h"

#include "Net/UnrealNetwork.h"

void AMy_FireBall::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMy_FireBall, ReturnToActor);
	DOREPLIFETIME(AMy_FireBall, ApexLocation);
}

void AMy_FireBall::BeginPlay()
{
	Super::BeginPlay();

	/*
	 * 服务器：ApexLocation 在 SpawnFireBalls 里已经算好，直接开播。
	 * 客户端：此刻复制数据还没到，ApexLocation 仍是 (0,0,0)，
	 *         硬开会朝世界原点飞 —— 交给 OnRep_ApexLocation 触发。
	 */
	if (HasAuthority())
	{
		StartOutgoingTimeline();
	}
}

void AMy_FireBall::OnRep_ApexLocation()
{
	StartOutgoingTimeline();
}

void AMy_FireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}
