// Copyright ABiao


#include "My_EffectActor/My_AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AMy_AuraEffectActor::AMy_AuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AMy_AuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AMy_AuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	//ApplyEffectToTarget()主要使用在碰撞检测中，如果target没有ASC直接返回
	if (TargetASC == nullptr) return;
	//这个GameplayEffectClass在蓝图设置，没设置报错
	check(GameplayEffectClass);


	//创建EffectContextHandle,用来封装EffectContext相关信息
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	//创建FGameplayEffectSpec，一种动态的Effect，可以修改之后再apply给Actor
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContext);


	//使用FGameplayEffectSpec给Actor
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//通过EffectSpecHandle判断Effect是什么类型，Instant，Infinite，HasDuration
	bool IsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	//如果是Infinite,把ActiveGameplayEffectHandle和这个TargetASC联系起来方便后面删除
	if (IsInfinite&& InfiniteEffectRemovePolicy == My_EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActorToActiveEffect_Map.Add(TargetASC, ActiveEffectHandle);
	}
}

//各种Effect在覆盖时启用
void AMy_AuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

}

//各种Effect在覆盖结束启用
void AMy_AuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == My_EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	if (InfiniteEffectRemovePolicy == My_EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (TargetASC == nullptr) return;

		TArray<UAbilitySystemComponent*> HandToRemove;
		for (auto m : ActorToActiveEffect_Map)
		{
			//当前TargetASC处于移除对中
			if (TargetASC == m.Key)
			{
				//移除Infinite Effect
				TargetASC->RemoveActiveGameplayEffect(m.Value, 1);

				//不能在for(auto:)循环中删除这个键值对,保存先后面删除
				HandToRemove.Add(TargetASC);
			}
		}

		for (auto& c : HandToRemove)
		{
			ActorToActiveEffect_Map.FindAndRemoveChecked(c);
		}
	}
}


