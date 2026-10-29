// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Aura/Public/My_AbilitySystem/Ability/My_AuraDamageGameplayAbility.h"
#include "My_AuraBeamSpell.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraBeamSpell : public UMy_AuraDamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StoreMouseDataInfo(const FHitResult& HitResult);
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDie(AActor* DeadActor);
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDie(AActor* DeadActor);
protected:
	UPROPERTY(BlueprintReadWrite, Category = "My_Ability|Beam")
	FVector MouseHitLocation;

	UPROPERTY(BlueprintReadWrite, Category = "My_Ability|Beam")
	TObjectPtr<AActor> MouseHitActor;

	UPROPERTY(BlueprintReadWrite, Category = "My_Ability|Beam")
	TObjectPtr<APlayerController> OwnerController;

	UPROPERTY(BlueprintReadWrite, Category = "My_Ability|Beam")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "My_Ability|Beam")
	int32 MaxNumShockTargets = 5;
};
