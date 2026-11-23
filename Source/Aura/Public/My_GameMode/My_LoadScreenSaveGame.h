// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "My_LoadScreenSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_LoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	/* 存档保存需要的变量*/
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	/* 存档保存的具体数据*/
	UPROPERTY()
	FString PlayerName = FString("Default Name");
};
