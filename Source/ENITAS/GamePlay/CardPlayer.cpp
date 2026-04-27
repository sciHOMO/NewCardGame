#include "../GamePlay/CardPlayer.h"
#include "../CardCoreSystem/CardCoreDriver.h"
#include "GameFramework/PlayerState.h"
#include "Camera/CameraActor.h"
#include "../SaveGame/LocalDeckSettings.h"
#include "../Client/EventListener.h"
#include "../Client/CardModel.h"
#include "../Client/MainUMG.h"
#include "../Misc/MiscFunctionLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ACardPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	if (GetNetMode() != NM_DedicatedServer)
	{
		SetLocalElement();	//准备本地元素
		
		EventListener = NewObject<UEventListener>(this, EventListenerClass);
		EventListener -> RegisterComponent();	//创建事件聆听器
		EventListener -> Controller = this;
	}
}

void ACardPlayer::Tick(float DeltaSeconds)
{
	if (EventListener)
	{
		EventListener -> CheckQueue();
	}
}

//****************************本地前置效果*******************************	
void ACardPlayer::SetLocalElement()
{
	if (PlayerState && PlayerState -> GetPlayerId() != INT_ERROR)
	{
		SetViewTarget(UGameplayStatics::GetActorOfClass(this, ACameraActor::StaticClass()));	//摄像机绑定
		if (UMainUMG* Widget = CreateWidget<UMainUMG>(GetWorld(), MainUMGClass))	//UMG生成
		{
			MainUMG = Widget;
			Widget -> AddToViewport();
		}
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorldTimerManager() . SetTimer(TimerHandle, this, &ACardPlayer::SetLocalElement, 0.1F);
	}
}

void ACardPlayer::FindLocalDeck()
{

	
	if (const USaveGame* LocalDeck = UGameplayStatics::LoadGameFromSlot(TEXT("LocalDeckSettings"), 0))
	{
		if (const ULocalDeckSettings* FoundLocalDeck = Cast<ULocalDeckSettings>(LocalDeck))
		{
			SendLocalDeck(FoundLocalDeck -> FindDeckByIndex(1));
		}
	}
}

void ACardPlayer::SendLocalDeck_Implementation(const TArray<int>& Deck)
{
	if (PlayerState && PlayerState -> GetPlayerId() != INT_ERROR)
	{
		Cast<ACardCoreDriver>(GetWorld() -> GetGameState()) -> CallBackForDeck(PlayerState -> GetPlayerId(), Deck);	//复制牌组
	}
}
//*************************************************************************

//***************************客户端行动请求*******************************
void ACardPlayer::RequestEndTurn_Implementation()
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveEndTurn(PlayerState -> GetPlayerId());
	}
}

void ACardPlayer::RequestPlayCard_Implementation(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState())  -> ReceivePlayCard(PlayerState -> GetPlayerId(), SourceCard, TargetCard, Sacrifice);
	}
}

void ACardPlayer::RequestAttack_Implementation(const int SourceCard, const int TargetCard)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveAttack(PlayerState -> GetPlayerId(), SourceCard, TargetCard);
	}
}

void ACardPlayer::RequestActivate_Implementation(const int Card, const TArray<int>& Sacrifice)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveActivate(PlayerState -> GetPlayerId(), Card, Sacrifice);
	}
}

void ACardPlayer::RequestTarget_Implementation(const int PickedTarget)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveTarget(PlayerState -> GetPlayerId(), PickedTarget);
	}
}
//*************************************************************************

void ACardPlayer::SetInputMode(EInputMode NewMode)
{
	switch(NewMode)
	{
	case EInputMode::Idle :
		{
			if (FocusActor)
			{
				FocusActor -> SetCardState(EState::Lerp);
				FocusActor = nullptr;
			}
			if (SecondFocusActor)
			{
				SecondFocusActor -> SetCardState(EState::Lerp);
				SecondFocusActor = nullptr;
			}
			break;
		}
	case  EInputMode::PlayCard :
		{
			FocusActor = CheckHitResult();
			if (FocusActor)
			{
				FocusActor -> SetCardState(EState::Follow);
			}
			break;
		}
	case EInputMode::AttackOrActivate :
		{
			break;
		}
	case EInputMode::Attack :
		{
			break;
		}
	case EInputMode::Activate :
		{
			break;
		}
	case EInputMode::PickUpSacrifices :
		{
			MainUMG -> NotifyPickUpSacrifice(FocusActor -> CardStruct.CardLevel);
			FocusActor -> SetCardState(EState::Hide);
			break;
		}
	case EInputMode::PickUpTargets :
		{
			break;
		}
	default : break;	
	}
	InputMode = NewMode;
}

void ACardPlayer::LeftMouseButtonClicked()
{
	switch(InputMode)
	{
	case EInputMode::Idle :
		{
			if (CheckHitResult())
			{
				if (CheckHitResult() -> CardStruct.CardZone == EZone::HandZone && CheckHitResult() -> CardStruct.PlayerIndex == PlayerState -> GetPlayerId())
				{
					SetInputMode(EInputMode::PlayCard); break;
				}
				if (CheckHitResult() -> CardStruct.CardZone == EZone::BoardZone && CheckHitResult() -> CardStruct.PlayerIndex == PlayerState -> GetPlayerId())
				{
					SetInputMode(EInputMode::AttackOrActivate); break;
				}
			}
			break;
		}
	case EInputMode::AttackOrActivate :
		{
			if (CheckHitResult() && CheckHitResult() == FocusActor)
			{
				SetInputMode(EInputMode::Activate);
			}
			break;
		}
	case EInputMode::PickUpSacrifices :
		{
			if (CheckHitResult() && CheckHitResult() != FocusActor)
			{
				if (!SacrificeMap.Contains(CheckHitResult() -> CardStruct.CardID))
				{
					CheckHitResult() -> SetCardState(EState::Focus);
					SacrificeMap.Emplace(CheckHitResult() -> CardStruct.CardID, CheckHitResult());
				}
				else
				{
					CheckHitResult() -> SetCardState(EState::Lerp);
					SacrificeMap.Remove(CheckHitResult() -> CardStruct.CardID);
				}
				MainUMG -> NotifyPickUpCount(SacrificeMap.Num());
			}
			break;
		}
	case EInputMode::PickUpTargets :
		{
			//添加对象并检测是否完成
			break;
		}
	default : break;	
	}
}

void ACardPlayer::LeftMouseButtonReleased()
{
	switch(InputMode)
	{
	case EInputMode::PlayCard :
		{
			if (CheckInsideBoard())
			{
				if (FocusActor -> CardStruct.CardLevel == 0)
				{
					RequestPlayCard(FocusActor -> CardStruct.CardIndex, INT_ERROR, {});
					SetInputMode(EInputMode::Idle);
				}
				else
				{
					SetInputMode(EInputMode::PickUpSacrifices);
				}
			}
			else
			{
				SetInputMode(EInputMode::Idle);
			}
			break;
		}
	case EInputMode::AttackOrActivate :
		{
			if (CheckHitResult() && CheckHitResult() -> CardStruct.CardZone == EZone::BoardZone && FocusActor -> CardStruct.PlayerIndex != PlayerState -> GetPlayerId())
			{
				SetInputMode(EInputMode::Attack);
			}
			break;
		}
	case EInputMode::Activate :
		{
			SetInputMode(EInputMode::PickUpSacrifices);
			break;
		}
	case EInputMode::PickUpSacrifices :
		{
			//添加对象并检测是否完成
			break;
		}
	case EInputMode::PickUpTargets :
		{
			//添加对象并检测是否完成
			break;
		}
	default : break;	
	}
}

void ACardPlayer::CallBackPickUpSacrifice()
{
	TArray<int> Result;
	SacrificeMap.GenerateKeyArray(Result);
	SacrificeMap.Empty();
	
	RequestPlayCard(FocusActor -> CardStruct.CardIndex, INT_ERROR, Result);
	SetInputMode(EInputMode::Idle);
}

ACardModel* ACardPlayer::CheckHitResult() const
{
	AActor* HitActor = nullptr;
	UMiscFunctionLibrary::GetMouseOverActor(this, HitActor);
	if (HitActor && Cast<ACardModel>(HitActor)) return Cast<ACardModel>(HitActor);
	return nullptr;
}

bool ACardPlayer::CheckInsideBoard() const
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_GameTraceChannel1, true, HitResult);
	return HitResult.bBlockingHit;
}
