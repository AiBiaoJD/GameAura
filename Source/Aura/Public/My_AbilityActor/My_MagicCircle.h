// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "My_MagicCircle.generated.h"

class UMaterialInterface;

UCLASS()
class AURA_API AMy_MagicCircle : public AActor
{
	GENERATED_BODY()

public:
	AMy_MagicCircle();
	virtual void Tick(float DeltaTime) override;

	/** 换贴花材质（BONUS 项）：已显示时也能中途换，不用 Hide 再 Show */
	void SetMagicCircleMaterial(UMaterialInterface* DecalMaterial);

protected:
	virtual void BeginPlay() override;

	// 空场景根：只作为变换锚点，避免 Decal 的投影体积撑大 Actor 包围盒
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;
};
