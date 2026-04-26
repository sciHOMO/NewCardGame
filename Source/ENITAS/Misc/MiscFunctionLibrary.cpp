#include "../Misc/MiscFunctionLibrary.h"

void UMiscFunctionLibrary::GetMouseProjectileLocationAtHeight(APlayerController* PC, int Height, FVector& Location)
{
	if (!PC)
	{
		Location = FVector::ZeroVector; return;
	}
	
	if (FVector WorldLocation, WorldDirection; PC-> DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
	{
		
		const float T = (Height - WorldLocation.Z) / WorldDirection.Z;
		if (T >= 0)
		{
			Location = WorldLocation + WorldDirection * T;
			return;
		}
	}
	Location = FVector::ZeroVector; return;
}

void UMiscFunctionLibrary::GetMouseOverLocation(const APlayerController* PC, FVector& CursorLocation, FVector& ActorLocation)
{
	FVector Direction = FVector::ZeroVector;
	PC -> DeprojectMousePositionToWorld(CursorLocation, Direction);
}

void UMiscFunctionLibrary::GetMouseOverActor(const APlayerController* PC, AActor*& Actor)
{
	if (FHitResult HitResult; PC -> GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, HitResult))
	{
		Actor = HitResult.GetActor();
	}
}
