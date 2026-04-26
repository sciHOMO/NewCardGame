#include "../Client/CardModel.h"
#include "../Client/EventListener.h"
#include "../Misc/MiscFunctionLibrary.h"
#include "ENITAS/GamePlay/CardPlayer.h"

ACardModel::ACardModel()
{
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	RootComponent = Root;
	CardCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CardCollision"));
	CardCollision -> SetupAttachment(Root);
	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CardMesh"));
	CardMesh -> SetupAttachment(Root);
}

void ACardModel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACardModel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (CardState == EState::Lerp)
	{
		LerpToTargetLocation(12.0F);
	}
	if (CardState == EState::Focus)
	{
		LerpToTargetLocation(12.0F);
	}
	if (CardState == EState::Follow)
	{
		UMiscFunctionLibrary::GetMouseProjectileLocationAtHeight(Cast<APlayerController>(EventListener -> Controller), 240.0F, TargetLocation);
		LerpToTargetLocation(25.0F);
	}
}

void ACardModel::SetCardState(EState NewState)
{
	switch(NewState)
	{
	case EState::Anim :
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(false);
			break;
		}
	case EState::Lerp	:
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);
			AdditiveLocation = FVector::ZeroVector;
			if (CardState == EState::Follow)
			{
				EventListener -> AddToHandZone(this);
				EventListener -> RefreshHandZone(CardStruct.PlayerIndex);
				break;
			}
			break;
		}
	case EState::Follow:
		{
			if (CardState == EState::Lerp || CardState == EState::Focus)
			{
				EventListener -> RemoveFromHandZone(this);
				EventListener -> RefreshHandZone(CardStruct.PlayerIndex);
				break;
			}
			break;
		}
	case EState::Focus :
		{
			if (CardStruct.CardZone == EZone::HandZone && CardStruct.PlayerIndex == EventListener -> Controller -> PlayerState -> GetPlayerId())
			{
				AdditiveLocation = FVector(0.0F, 30.0f, 30.0F);
			}
			if (CardStruct.CardZone == EZone::EchoZone && CardStruct.PlayerIndex == EventListener -> Controller -> PlayerState -> GetPlayerId())
			{
				AdditiveLocation = FVector(0.0F, 30.0f, 30.0F);
			}
			if (CardStruct.CardZone == EZone::BoardZone)
			{
				AdditiveLocation = FVector(0.0F, 0.0f, 30.0F);
			}
			break;
		}
	case EState::Hide:
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
			if (CardState == EState::Follow)
			{
				EventListener -> RemoveFromHandZone(this);
				EventListener -> RefreshHandZone(CardStruct.PlayerIndex);
				break;
			}
			break;
		}
	}
	CardState = NewState;
}

void ACardModel::LerpToTargetLocation(const float LerpSpeed)
{
	const FVector CurrentLocation = GetActorLocation();
	const FVector CurrenScale = GetActorScale3D();
	const FVector NextLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld() -> DeltaTimeSeconds, LerpSpeed);
	const FVector NextScale = FMath::Lerp(CurrenScale, FVector::OneVector, 0.2f);
	SetActorLocation(NextLocation);
	SetActorScale3D(NextScale);

	const FVector CurrentAdditiveLocation = CardMesh -> GetRelativeLocation();
	const FVector NextAdditiveLocation = FMath::VInterpTo(CurrentAdditiveLocation, AdditiveLocation, GetWorld() -> DeltaTimeSeconds, 12.0F);
	CardMesh -> SetRelativeLocation(NextAdditiveLocation);
}

void ACardModel::StartDrawCard_Implementation(const bool Owning, const FEventPackageStruct& Package)
{
	PackageStruct = Package;
	CardStruct = Package.Params[0].CardOrPlayer;
	SetCardState(EState::Anim);
}

void ACardModel::EndDrawCard()
{
	EventListener -> AddToHandZone(this);
	EventListener -> RefreshHandZone(CardStruct.PlayerIndex);
	EventListener -> Clear(PackageStruct.GlobalEventIndex);
	SetCardState(EState::Lerp);
}

void ACardModel::StartSummonServant_Implementation(const bool Owning, const FEventPackageStruct& Package)
{
	PackageStruct = Package;
	CardStruct = Package.Params[0].CardOrPlayer;
	SetCardState(EState::Anim);
}

void ACardModel::EndSummonServant()
{
	EventListener -> RemoveFromHandZone(this);
	EventListener -> AddToBoardZone(this);
	EventListener -> RefreshBoardZone(CardStruct.PlayerIndex);
	EventListener -> Clear(PackageStruct.GlobalEventIndex);
	SetCardState(EState::Lerp);
}

void ACardModel::StartCastSpell_Implementation(const bool Owning, const FEventPackageStruct& Package)
{
	PackageStruct = Package;
	CardStruct = Package.Params[0].CardOrPlayer;
	SetCardState(EState::Anim);
}

void ACardModel::EndCastSpell()
{
	EventListener -> RemoveFromHandZone(this);
	if (CardStruct.CardZone == EZone::GraveZone)
	{
		SetCardState(EState::Hide);
	}
	else if (CardStruct.CardZone == EZone::EchoZone)
	{
		EventListener -> AddToEchoZone(this);
		EventListener -> RefreshEchoZone(CardStruct.PlayerIndex);
		SetCardState(EState::Lerp);
	}
	EventListener -> Clear(PackageStruct.GlobalEventIndex);
}



