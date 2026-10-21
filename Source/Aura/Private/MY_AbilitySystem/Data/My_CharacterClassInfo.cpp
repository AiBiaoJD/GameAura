// Copyright ABiao


#include "Aura/Public/My_AbilitySystem/Data/My_CharacterClassInfo.h"

FMy_CharacterClassDefaultInfo UMy_CharacterClassInfo::GetClassDefaultInfo(EMy_CharacterClass CharacterClass)
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
