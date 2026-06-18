// Copyright ABiao


#include "MY_AbilitySystem/Ability/My_AuraSummonAbility.h"

TArray<FVector> UMy_AuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();

	const FVector RightBoundary = Forward.RotateAngleAxis(SpawnSpread / 2.f, FVector::UpVector);
	const FVector LeftBoundary = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);

	TArray<FVector> SpawnLocations;

	for (int32 i = 0; i < NumMinions; i++)
	{
		FVector Direction;
		if (NumMinions == 1)
		{
			Direction = Forward;
		}
		else
		{
			const float Alpha = (float)i / (float)(NumMinions - 1);
			Direction = FMath::Lerp(LeftBoundary, RightBoundary, Alpha).GetSafeNormal();
		}

		const float RandomDistance = FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		const FVector HorizontalLocation = Location + Direction * RandomDistance;

		const FVector Start = HorizontalLocation + FVector(0, 0, 500.f);
		const FVector End   = HorizontalLocation - FVector(0, 0, 500.f);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

		FVector GroundLocation;
		if (Hit.bBlockingHit)
		{
			GroundLocation = Hit.Location;
		}
		else
		{
			GroundLocation = HorizontalLocation;
		}

		SpawnLocations.Add(GroundLocation);
	}

	return SpawnLocations;
}
