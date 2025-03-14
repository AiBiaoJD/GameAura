// Copyright ABiao


#include "My_AuraGamePlayTags_Singleton.h"

#include "GameplayTagsManager.h"


FMy_AuraGameplayTags FMy_AuraGameplayTags::GameplayTagsInstance;
void FMy_AuraGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTagsInstance.My_Attribute_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("My_Attribute.Secondary.Armor"), FString("reduce Damage and Increase Block Chance"));
}
