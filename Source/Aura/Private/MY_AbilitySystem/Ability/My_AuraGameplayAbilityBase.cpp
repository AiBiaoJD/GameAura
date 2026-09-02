// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"

FString UMy_AuraGameplayAbilityBase::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"DefaultAbilityName - LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum", Level);
}

FString UMy_AuraGameplayAbilityBase::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>NextLevel: </><Level>%d</> \n<Default>Cause more Damage</>"), Level);
}

FString UMy_AuraGameplayAbilityBase::GetLockDescription(int32 LevelRequirement)
{
	return FString::Printf(TEXT("<Default>Ability Spell Locked until level: </><Level>%d</>"), LevelRequirement);
}
