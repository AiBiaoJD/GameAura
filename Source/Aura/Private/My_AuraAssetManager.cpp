// Copyright ABiao


#include "My_AuraAssetManager.h"

#include "AbilitySystemGlobals.h"
#include "My_AuraGamePlayTags_Singleton.h"

UMy_AuraAssetManager& UMy_AuraAssetManager::GetInstance()
{
	check(GEngine);

	UMy_AuraAssetManager* AuraAssetManager = Cast<UMy_AuraAssetManager>(GEngine->AssetManager);

	return  *AuraAssetManager;

}

void UMy_AuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//添加Tags,在加载资源的时候
	FMy_AuraGameplayTags::InitializeNativeGameplayTags();

	// This is Required to Use TargetData
	UAbilitySystemGlobals::Get().InitGlobalData();
}


