// Copyright ABiao


#include "MY_AbilitySystem/Debuff/My_DebuffNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "My_Interraction/My_CombatInterface.h"

UMy_DebuffNiagaraComponent::UMy_DebuffNiagaraComponent()
{
	bAutoActivate = false;
}

void UMy_DebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	IMy_CombatInterface* CombatInterface = Cast<IMy_CombatInterface>(GetOwner());
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	// ★ 死亡通知：和 ASC 有没有就绪无关，先订上,
	// 在actor死亡那一刻，★ 死亡时 Tag 没变（GE 还没到期）
	if (CombatInterface)
	{
		CombatInterface->GetOnDeath().AddDynamic(this, &UMy_DebuffNiagaraComponent::OnOwnerDeath);
	}

	if (ASC)
	{
		// 情况 A：BeginPlay 时 ASC 已经就绪 —— 直接注册监听
		ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved)
		   .AddUObject(this, &UMy_DebuffNiagaraComponent::DebuffTagChanged);
	}
	// 情况 B：本组件的 BeginPlay 可能【早于】ASC 初始化
	//   （玩家的 ASC 要等 OnRep_PlayerState 才就绪，见第 46 章）
	//   → 挂在"ASC 就绪"的委托上等它，而不是每帧轮询
	else if (CombatInterface)
	{
		// ★ 这里必须用 AddWeakLambda，不能用 AddLambda：
		//   lambda 里 [this] 捕获的 this 【不被 GC 追踪】（不是 UPROPERTY），
		//   而这个委托挂在【角色】身上，很可能比本组件活得久。
		//   用 AddLambda 的话，组件销毁后委托仍持有这个 lambda，
		//   角色下次广播时会执行它 → 解引用已销毁的 this → 野指针崩溃。
		//   AddWeakLambda 内部额外存一个 TWeakObjectPtr，广播时先判 IsValid()，
		//   无效就静默跳过（见 DelegateInstancesImpl.h:835 ExecuteIfSafe）。
		CombatInterface->GetOnASCRegistered().AddWeakLambda(this, [this](UAbilitySystemComponent* InASC)
		{
			InASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UMy_DebuffNiagaraComponent::DebuffTagChanged);
		});
	}
}

void UMy_DebuffNiagaraComponent::DebuffTagChanged(const FGameplayTag Callbacktag, int32 NewCount)
{
	// ★ 额外检查"主人还活着吗"：
	//   Debuff 的 GE 有可能比角色活得久（角色死了但 Duration 还没到），
	//   不加这个判断会出现"尸体还在烧"的残影。
	//   发生在Tag 变化时，死后才发生 Tag 变化（延迟 GE 加 Tag）
	// ⚠️ 注意 Implements<> 里必须写【U 开头的类】（UMy_CombatInterface），不是 I 开头的：
	//    UObject::Implements<T>() 内部是 T::StaticClass()（Class.h:3785），
	//    而 StaticClass() 只有 UCLASS / UINTERFACE 类才有；
	//    IMy_CombatInterface 只是普通 C++ 类（接口的实现体），没有 StaticClass。
	//    对比：Cast<IMy_CombatInterface>() 用的是 I 类（内部走 InterfaceType::UClassType），
	//          IMy_CombatInterface::Execute_Xxx() 也用 I 类 —— 只有 Implements<> 要 U 类。
	const bool bOwnerValid = IsValid(GetOwner());
	const bool bOwnerAlive = bOwnerValid
		&& GetOwner()->Implements<UMy_CombatInterface>()
		&& !IMy_CombatInterface::Execute_IsDead(GetOwner());

	if (NewCount > 0 && bOwnerAlive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void UMy_DebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
