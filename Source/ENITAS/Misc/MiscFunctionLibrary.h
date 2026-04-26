// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MiscFunctionLibrary.generated.h"

/**
 * 
 */

UCLASS()
class UMiscFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION()
	static void GetMouseProjectileLocationAtHeight(APlayerController* PC, int Height,  FVector& Location);
	
	UFUNCTION()
	static void GetMouseOverLocation(const APlayerController* PC,  FVector& CursorLocation, FVector& ActorLocation);
	
	UFUNCTION()
	static void GetMouseOverActor(const APlayerController* PC, AActor*& Actor);
};
