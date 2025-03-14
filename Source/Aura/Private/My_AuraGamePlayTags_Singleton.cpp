// Copyright ABiao


#include "My_AuraGamePlayTags_Singleton.h"

#include "GameplayTagsManager.h"

FMy_AuraGameplayTags FMy_AuraGameplayTags::GameplayTagsInstance;
void FMy_AuraGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.Armor"), FString("护甲:主要减少受到的伤害百分比,并提高Block Chance的概率(受到伤害减半)和敌人的暴击伤害"));
}
