#include "../GamePlay/AIPlayer.h"
#include "../CardCoreSystem/CardCoreDriver.h"
#include "../Client/EventListener.h"
#include "../SaveGame/LocalDeckSettings.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"

void AAIPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		FindLocalDeck();
		
		EventListener = NewObject<UEventListener>(this, EventListenerClass);
		EventListener -> RegisterComponent();	//创建事件聆听器
		EventListener -> Controller = this;
	}
}

void AAIPlayer::Tick(float DeltaSeconds)
{
	if (EventListener)
	{
		EventListener -> CheckQueue();
	}
}

void AAIPlayer::FindLocalDeck()
{
	if (PlayerState && PlayerState -> GetPlayerId() != INT_ERROR)
	{
		if (const USaveGame* LocalDeck = UGameplayStatics::LoadGameFromSlot(TEXT("LocalDeckSettings"), 0))
		{
			if (const ULocalDeckSettings* FoundLocalDeck = Cast<ULocalDeckSettings>(LocalDeck))
			{
				SendLocalDeck(FoundLocalDeck -> FindDeckByIndex(1));
			}
		}
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager() . SetTimer(TimerHandle, this, &AAIPlayer::FindLocalDeck, 0.1F);
	}
}

//****************************本地前置效果*******************************	
void AAIPlayer::SendLocalDeck_Implementation(const TArray<int>& Deck)
{
	if (PlayerState && PlayerState -> GetPlayerId() != INT_ERROR)
	{
		Cast<ACardCoreDriver>(GetWorld() -> GetGameState()) -> CallBackForDeck(PlayerState -> GetPlayerId(), Deck);
	}
}
//*************************************************************************

//***************************客户端行动请求*******************************
void AAIPlayer::RequestEndTurn_Implementation()
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveEndTurn(PlayerState -> GetPlayerId());
	}
}

void AAIPlayer::RequestPlayCard_Implementation(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceivePlayCard(PlayerState -> GetPlayerId(), SourceCard, TargetCard, Sacrifice);
	}
}

void AAIPlayer::RequestAttack_Implementation(const int SourceCard, const int TargetCard)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveAttack(PlayerState -> GetPlayerId(), SourceCard, TargetCard);
	}
}

void AAIPlayer::RequestActivate_Implementation(const int Card, const TArray<int>& Sacrifice)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveActivate(PlayerState -> GetPlayerId(), Card, Sacrifice);
	}
}

void AAIPlayer::RequestTarget_Implementation(const int PickedTarget)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveTarget(PlayerState -> GetPlayerId(), PickedTarget);
	}
}
//*************************************************************************