// Copyright ABiao


#include "My_AbilityActor/My_PointCollection.h"

#include "Aura/Public/My_AbilitySystem/My_AuraAbilitySystemLibrary.h"
#include "Engine/Texture2D.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"

AMy_PointCollection::AMy_PointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	/*
	 * 点位的编辑器图标
	 * Billboard 只有赋了 Sprite 才会在视口里画出来，这里先用引擎自带的棋盘格占位，
	 * 想要好看的小圆点在 BP 里覆盖 Sprite 即可
	 */
	static ConstructorHelpers::FObjectFinder<UTexture2D> PointSprite(TEXT("/Engine/EngineResources/DefaultTexture.DefaultTexture"));

	/*
	 * 11 个点位
	 * CreateDefaultSubobject 的名字必须是编译期常量且唯一，所以不能循环拼字符串，只能一个个写
	 * 每建一个顺手塞进 ImmutablePts，数组顺序就是 PT_0 -> PT_10
	 */
	PT_0 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_0"));
	ImmutablePts.Add(PT_0);
	SetRootComponent(PT_0);

	PT_1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_1"));
	ImmutablePts.Add(PT_1);
	PT_1->SetupAttachment(PT_0);

	PT_2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_2"));
	ImmutablePts.Add(PT_2);
	PT_2->SetupAttachment(PT_0);

	PT_3 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_3"));
	ImmutablePts.Add(PT_3);
	PT_3->SetupAttachment(PT_0);

	PT_4 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_4"));
	ImmutablePts.Add(PT_4);
	PT_4->SetupAttachment(PT_0);

	PT_5 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_5"));
	ImmutablePts.Add(PT_5);
	PT_5->SetupAttachment(PT_0);

	PT_6 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_6"));
	ImmutablePts.Add(PT_6);
	PT_6->SetupAttachment(PT_0);

	PT_7 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_7"));
	ImmutablePts.Add(PT_7);
	PT_7->SetupAttachment(PT_0);

	PT_8 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_8"));
	ImmutablePts.Add(PT_8);
	PT_8->SetupAttachment(PT_0);

	PT_9 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_9"));
	ImmutablePts.Add(PT_9);
	PT_9->SetupAttachment(PT_0);

	PT_10 = CreateDefaultSubobject<UBillboardComponent>(TEXT("PT_10"));
	ImmutablePts.Add(PT_10);
	PT_10->SetupAttachment(PT_0);

	// 统一设置：给图标、并禁止在游戏里渲染（纯编辑器标记）
	for (USceneComponent* Pt : ImmutablePts)
	{
		UBillboardComponent* Billboard = Cast<UBillboardComponent>(Pt);
		if (!Billboard) continue;

		Billboard->Sprite = PointSprite.Object;
		Billboard->bHiddenInGame = true;
	}
}

void AMy_PointCollection::CacheOriginalOffsets()
{
	// 已经缓存过就别再覆盖：这时组件位置可能已经被上一次贴地改过了
	if (OriginalOffsets.Num() == ImmutablePts.Num()) return;

	OriginalOffsets.Reset(ImmutablePts.Num());
	for (const TObjectPtr<USceneComponent>& Pt : ImmutablePts)
	{
		OriginalOffsets.Add(IsValid(Pt) ? Pt->GetRelativeLocation() : FVector::ZeroVector);
	}
}

TArray<USceneComponent*> AMy_PointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride)
{
	// 教程这里用 checkf，点数配少了（比如等级给 12 但只有 11 个点）会直接把游戏断言崩掉；
	// 这里夹取，最多就是用不满
	const int32 Count = FMath::Clamp(NumPoints, 0, ImmutablePts.Num());
	if (Count == 0) return TArray<USceneComponent*>();

	// 手摆偏移只取一次，之后每次调用都用同一份基准
	CacheOriginalOffsets();

	/*
	 * 查询放在循环外，只做一次
	 * 教程把 GetLivePlayersWithinRadius 写在了循环里，11 个点位就查 11 遍，参数还完全一样
	 */
	TArray<AActor*> IgnoreActors;
	UMy_AuraAbilitySystemLibrary::GetLivePlayersWithRadius(this, IgnoreActors, TArray<AActor*>(), LivePlayerSearchRadius, GetActorLocation());

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActors(IgnoreActors);

	TArray<USceneComponent*> Result;
	Result.Reserve(Count);

	for (int32 i = 0; i < Count; ++i)
	{
		USceneComponent* Pt = ImmutablePts[i];
		if (!IsValid(Pt)) continue;

		/*
		 * 点位 = 圆心 + 偏移
		 * OriginalOffsets 存的是局部偏移（相对 PT_0），RotateVector 按 Actor 朝向把它转成世界方向，
		 * 这样 Actor 转到哪边，图案就跟着转到哪边。
		 * PT_0 是圆心本身，绕自己转没意义，所以不参与 YawOverride。
		 * 基准全部来自缓存的手摆值，不读组件当前世界位置 —— 所以反复调用不会累积漂移。
		 */
		FVector Offset = GetActorQuat().RotateVector(OriginalOffsets[i]);
		if (i > 0)
		{
			Offset = Offset.RotateAngleAxis(YawOverride, FVector::UpVector);
		}

		const FVector Target = GroundLocation + Offset;

		FHitResult Hit;
		const bool bHit = GetWorld()->LineTraceSingleByProfile(
			Hit,
			Target + FVector(0.f, 0.f, TraceHeight),
			Target - FVector(0.f, 0.f, TraceHeight),
			FName("BlockAll"),
			QueryParams);

		// 打空（悬空、坑洞）就留在平面上，别把点位扔到世界原点
		Pt->SetWorldLocation(bHit ? FVector(Target.X, Target.Y, Hit.ImpactPoint.Z) : Target);

		// 朝地面法线，Niagara 的发射方向才跟着坡度走
		if (bHit)
		{
			Pt->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(Hit.ImpactNormal));
		}

		Result.Add(Pt);
	}

	return Result;
}

void AMy_PointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}
