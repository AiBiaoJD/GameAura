// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "My_DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_DebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UMy_DebuffNiagaraComponent();

	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DebuffTag;

protected:

	virtual void BeginPlay() override;
	void DebuffTagChanged(const FGameplayTag Callbacktag, int32 NewCount);

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
