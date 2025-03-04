// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "My_AuraEffectActor.generated.h"

//Effect启用的规则
UENUM(BlueprintType)
enum class My_EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply
};

//Effect移除的规则
//Instant 和 Duration 自动移除，只有Infinite需要手动移除
UENUM(BlueprintType)
enum class My_EEffectRemovePolicy
{
	RemoveOnEndOverlap,
	DoNotRemove
};


UCLASS()
class AURA_API AMy_AuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AMy_AuraEffectActor();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);



	//Effect是否在移除后销毁Actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	bool bDestroyOnEffectRemove = false;

	//Instant Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	My_EEffectApplicationPolicy InstantEffectApplicationPolicy = My_EEffectApplicationPolicy::DoNotApply;

	//Duration Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	My_EEffectApplicationPolicy DurationEffectApplicationPolicy = My_EEffectApplicationPolicy::DoNotApply;

	//Infinite Effect
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	My_EEffectApplicationPolicy InfiniteEffectApplicationPolicy = My_EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied Effects")
	My_EEffectRemovePolicy InfiniteEffectRemovePolicy = My_EEffectRemovePolicy::RemoveOnEndOverlap;

	//键值对，为了移除Infinite Effect,保存Active Effect Handle和Actor的对应关系
	//一个Effect被多个Actor激活，会创建多个Hand，比如Hand1，Hand2..
	TMap<UAbilitySystemComponent*, FActiveGameplayEffectHandle> ActorToActiveEffect_Map;
private:

};

