// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

class UMy_LoadScreenSaveGame;
class UMy_MVVM_LoadSlot;
class UMy_AbilityInfo;
class UMy_CharacterClassInfo;
/**
 * 
 */
UCLASS()
class AURA_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category="Character Class Defaults")
	TObjectPtr<UMy_CharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly,Category="Ability Info Defaults")
	TObjectPtr<UMy_AbilityInfo> AbilityInfo;


	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMy_LoadScreenSaveGame> LoadScreenSaveGameClass;

	void SaveSlotData(UMy_MVVM_LoadSlot* LoadSlot);

};
