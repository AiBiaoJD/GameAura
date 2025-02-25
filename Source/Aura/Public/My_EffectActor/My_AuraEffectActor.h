// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "My_AuraEffectActor.generated.h"

UCLASS()
class AURA_API AMy_AuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AMy_AuraEffectActor();

	UFUNCTION()
	virtual void OnOverLap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()

	virtual void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	TObjectPtr<UStaticMeshComponent> Mesh;
};

