// Copyright ABiao


#include "MY_AbilitySystem/Data/My_LevelUpInfo.h"

int32 UMy_LevelUpInfo::FindLevelForXP(int32 XP) const
{
	for (int i = 1; i < LevelUpInformation.Num(); i++)
	{
		if (XP < LevelUpInformation[i].LevelUpRequirement)
		{
			return i;
		}
	}
	return LevelUpInformation.Num();
}
