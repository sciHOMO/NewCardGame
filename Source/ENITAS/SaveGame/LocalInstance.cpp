#include "../SaveGame/LocalInstance.h"
#include "../SaveGame/LocalDeckSettings.h"
#include "Kismet/GameplayStatics.h"

void ULocalInstance::Init()
{
	Super::Init();
	InitLocalDeckSettings();
}

void ULocalInstance::InitLocalDeckSettings()
{
	if (UGameplayStatics::LoadGameFromSlot(TEXT("LocalDeckSettings"), 0)) return;
	ULocalDeckSettings* NewLocalDeck = Cast<ULocalDeckSettings>(UGameplayStatics::CreateSaveGameObject(ULocalDeckSettings::StaticClass()));
	UGameplayStatics::SaveGameToSlot(NewLocalDeck, TEXT("LocalDeckSettings"), 0);
}
