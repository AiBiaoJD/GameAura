// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/Actor.h"
#include "My_PointCollection.generated.h"

/*
 * 法阵点位收集器
 * 手摆若干点位，GA 生成 Niagara 特效时按技能等级取前 N 个用
 */
UCLASS()
class AURA_API AMy_PointCollection : public AActor
{
	GENERATED_BODY()
	
public:	
	AMy_PointCollection();

	/*
	 * 把前 NumPoints 个点位吸到地面，返回组件本身
	 * 会改组件 transform，所以不是纯函数 —— 蓝图里只调一次，存进变量再进循环
	 *
	 * GroundLocation 图案圆心（法阵落点）
	 * NumPoints     用几个点位，按 PT_0 -> PT_10 顺序取
	 * YawOverride   整体绕 Z 轴额外旋转的角度
	 */
	UFUNCTION(BlueprintCallable)
	TArray<USceneComponent*> GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride = 0.0f);

protected:
	virtual void BeginPlay() override;

	/*
	 * 点位：PT_0 为 Root，PT_1 ~ PT_10 都挂在 PT_0 下面
	 * 用 Billboard 是为了在视口里能直接看到、拖动；它只是编辑器标记，游戏里不渲染
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_1;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_2;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_3;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_4;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_5;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_6;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_7;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_8;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_9;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBillboardComponent> PT_10;

	/*
	 * 上面 11 个点位的扁平列表，顺序就是 PT_0 -> PT_10
	 * 构造函数里填一次，之后只读。
	 * Immutable 指的是「成员和顺序不变」—— 里面的组件本身会被贴地逻辑改 transform
	 */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TArray<TObjectPtr<USceneComponent>> ImmutablePts;

	/*
	 * 手摆的原始偏移（相对 PT_0，Actor 局部空间）
	 * 首次调用时缓存一次，之后一直用这份基准。
	 * 不缓存的话就会像教程那样：读到的位置已被上一次贴地改过，同 Yaw 连调两次会越转越偏
	 */
	TArray<FVector> OriginalOffsets;

	/* 贴地射线往上、往下各探多远 */
	UPROPERTY(EditDefaultsOnly, Category = "My_PointCollection")
	float TraceHeight = 500.f;

	/* 查活着的玩家的半径：贴地时忽略他们，免得点位吸到敌人头上 */
	UPROPERTY(EditDefaultsOnly, Category = "My_PointCollection")
	float LivePlayerSearchRadius = 1500.f;

private:
	/* 把 ImmutablePts 里各点的 RelativeLocation 抓一份下来，只抓一次 */
	void CacheOriginalOffsets();

};
