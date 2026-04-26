#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LocalInstance.generated.h"

/*
 * 
 */
UCLASS()
class ULocalInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

public:

	static void InitLocalDeckSettings();
	
};
