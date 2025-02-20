// Copyright ABiao
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter_Base.generated.h"


UCLASS(ABSTRACT)
class AURA_API AMyCharacter_Base : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter_Base();

protected:
	virtual void BeginPlay() override;

	//��������
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
	
};
