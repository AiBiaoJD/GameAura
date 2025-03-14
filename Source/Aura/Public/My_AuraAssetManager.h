// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "My_AuraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static  UMy_AuraAssetManager& GetInstance();

protected:
	virtual void StartInitialLoading() override;

};
