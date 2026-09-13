// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraGameplayAbilityBase.h"

FString UMy_AuraGameplayAbilityBase::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default> %s, </><Level>%d</>"), TEXT("默认能力名字 -LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum LoremIpsum"), Level);
}

FString UMy_AuraGameplayAbilityBase::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>NextLevel:</><Level>%d</> \n<Default>造成更多伤害</>"), Level);
}

FString UMy_AuraGameplayAbilityBase::GetLockDescription(int32 LevelRequirement)
{
	return FString::Printf(TEXT("<Default>技能未解锁,需要等级:</><Level>%d</>"), LevelRequirement);
}
