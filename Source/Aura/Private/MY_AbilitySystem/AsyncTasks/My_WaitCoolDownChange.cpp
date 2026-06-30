// Copyright ABiao


#include "MY_AbilitySystem/AsyncTasks/My_WaitCoolDownChange.h"

#include "AbilitySystemComponent.h"

UMy_WaitCoolDownChange* UMy_WaitCoolDownChange::WaitCoolDownChange(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayTag& InCoolDownTag)
{
    UMy_WaitCoolDownChange* WaitCoolDownChange = NewObject<UMy_WaitCoolDownChange>();
    WaitCoolDownChange->ASC = AbilitySystemComponent;
    WaitCoolDownChange->CoolDownTag = InCoolDownTag;

    if (!IsValid(AbilitySystemComponent) || !InCoolDownTag.IsValid())
    {
        WaitCoolDownChange->EndTask();
        return nullptr;
    }

    // 冷却结束添加的回调函数
    AbilitySystemComponent->RegisterGameplayTagEvent(InCoolDownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(WaitCoolDownChange, &UMy_WaitCoolDownChange::CoolDownTagChanged);

    // 冷却开始添加的回调函数，这是为了获取时间从ASC上，所以不用上面的委托
    AbilitySystemComponent->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WaitCoolDownChange, &UMy_WaitCoolDownChange::OnActiveEffectAdded);
    
    return WaitCoolDownChange;
}

void UMy_WaitCoolDownChange::EndTask()
{
    if (!IsValid(ASC)) return;
    ASC->RegisterGameplayTagEvent(CoolDownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
    ASC->OnActiveGameplayEffectAddedDelegateToSelf.RemoveAll(this);
    SetReadyToDestroy();
    MarkAsGarbage();
}

void UMy_WaitCoolDownChange::CoolDownTagChanged(const FGameplayTag InCoolDownTag, int32 NewCount)
{
    // Tag移除了，冷却结束
    if (NewCount  == 0)
    {
        CoolDownEnd.Broadcast(0.f);
    }
}

void UMy_WaitCoolDownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
    FGameplayTagContainer AssetTags;
    Spec.GetAllAssetTags(AssetTags);
    FGameplayTagContainer GrantedTags;
    Spec.GetAllGrantedTags(GrantedTags);

    if (AssetTags.HasTagExact(CoolDownTag) || GrantedTags.HasTagExact(CoolDownTag))
    {
        FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CoolDownTag.GetSingleTagContainer());
        TArray<float> TTimeRemaing = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
        if (TTimeRemaing.Num() > 0)
        {
            float HighestTime = 0.0f;
            for (auto time : TTimeRemaing)
            {
                if (HighestTime < time)
                {
                    HighestTime = time;
                }
            }
            CoolDownStart.Broadcast(HighestTime);

        }        
        
    }
}
