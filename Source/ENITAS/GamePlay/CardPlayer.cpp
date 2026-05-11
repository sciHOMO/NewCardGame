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
#include "ENITAS/CardCoreSystem/EffectContext.h"
#include "ENITAS/CardCoreSystem/EffectInstance.h"
#include "ENITAS/Misc/RuleChecker.h"
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

void ACardPlayer::RequestPlayCard_Implementation(const int SourceCard, const int TargetCard, const TArray<int>& SacIndexArray)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState())  -> ReceivePlayCard(PlayerState -> GetPlayerId(), SourceCard, TargetCard, SacIndexArray);
	}
}

void ACardPlayer::RequestAttack_Implementation(const int SourceCard, const int TargetCard)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveAttack(PlayerState -> GetPlayerId(), SourceCard, TargetCard);
	}
}

void ACardPlayer::RequestActivate_Implementation(const int Card, const TArray<int>& SacIndexArray)
{
	if(GetWorld() -> GetGameState() && GetNetMode() != NM_Client)
	{
		Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()) -> ReceiveActivate(PlayerState -> GetPlayerId(), Card, SacIndexArray);
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
			FocusActor = CheckHitResult();
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
	case EInputMode::PickUpSacrificesForPlay :
		{
			MainUMG -> NotifyStartPickUpSacrifice(FocusActor -> CardStruct.CardSacrifice);
			FocusActor -> SetCardState(EState::Hide);
			break;
		}
	case EInputMode::PickUpSacrificesForEffect :
		{
			if (!FocusActor -> CardStruct.CardInstanceClass) return;
		
			const UCardInstance* CardCDO = FocusActor -> CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
			if (!CardCDO) return;

			const TSubclassOf<UEffectInstance>* EffectClassPtr = CardCDO->EffectForCondition.Find(ECondition::Activate);
			if (!EffectClassPtr || !(*EffectClassPtr)) return;

			const UEffectInstance* EffectCDO = (*EffectClassPtr) -> GetDefaultObject<UEffectInstance>();
			if (!EffectCDO) return;
			
			MainUMG -> NotifyStartPickUpSacrifice(EffectCDO -> EffectSacrifice);
			FocusActor -> SetCardState(EState::Focus);
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
	TArray<FCardStruct> List;
	for (ACardModel* Idx : EventListener -> AllCardModels)
	{
		List.Emplace(Idx -> CardStruct);
	}
	
	switch(InputMode)
	{
	case EInputMode::Idle :
		{
			if (CheckHitResult())
			{
				if (URuleChecker::CanPlayCard_Client(Cast<ACardCoreDriver>(GetWorld() -> GetGameState()), PlayerState -> GetPlayerId(), CheckHitResult() -> CardStruct, List))
				{
					SetInputMode(EInputMode::PlayCard); break;
				}
				if (URuleChecker::CanAttackOrActivate_Client(Cast<ACardCoreDriver>(GetWorld() -> GetGameState()), PlayerState -> GetPlayerId(), CheckHitResult() -> CardStruct))
				{
					SetInputMode(EInputMode::AttackOrActivate); break;
				}
			}
			break;
		}
	case EInputMode::AttackOrActivate :
		{
			if (CheckHitResult() && CheckHitResult() == FocusActor &&
				URuleChecker::CanActivate_Client(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), CheckHitResult() -> CardStruct, List))
				//和第一次点击的对象一致，未修改为Attack，且可支付
			{
				SetInputMode(EInputMode::Activate);
			}
			break;
		}
	case EInputMode::PickUpSacrificesForPlay :
		{
			if (CheckHitResult() && CheckHitResult() != FocusActor &&
				URuleChecker::IsValidSacrificeForPlay(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, CheckHitResult() -> CardStruct))
			{
				TArray<ACardModel*> Entity;
				if (!SacPickMap.Contains(CheckHitResult() -> CardStruct.CardIndex))
				{

					TArray<FCardStruct> SacStructArray;
					SacPickMap.GenerateValueArray(Entity);

					for (ACardModel* Idx : Entity)
					{
						SacStructArray.Emplace(Idx -> CardStruct);
					}
					
					if (!URuleChecker::IsNecessarySacrificesForPlay(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, SacStructArray))
					{
						CheckHitResult() -> SetCardState(EState::KeepFocus);
						SacPickMap.Emplace(CheckHitResult() -> CardStruct.CardIndex, CheckHitResult());
					}
				}
				else
				{
					CheckHitResult() -> SetCardState(EState::Lerp);
					SacPickMap.Remove(CheckHitResult() -> CardStruct.CardIndex);
				}
				
				TArray<FCardStruct> SacStructArray;
				SacPickMap.GenerateValueArray(Entity);

				for (ACardModel* Idx : Entity)
				{
					SacStructArray.Emplace(Idx -> CardStruct);
				}
				
				if (URuleChecker::IsNecessarySacrificesForPlay(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, SacStructArray))	//满足度检查
				{
					MainUMG -> NotifyEndPickUpSacrifice(true);
				}
				else
				{
					MainUMG -> NotifyEndPickUpSacrifice(false);
				}
			}
			break;
		}
	case EInputMode::PickUpSacrificesForEffect :
		{
			if (CheckHitResult() && CheckHitResult() != FocusActor &&
				URuleChecker::IsValidSacrificeForEffect(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, CheckHitResult() -> CardStruct))
			{
				TArray<ACardModel*> Entity;
				if (!SacPickMap.Contains(CheckHitResult() -> CardStruct.CardIndex))
				{
					
					TArray<FCardStruct> SacStructArray;
					SacPickMap.GenerateValueArray(Entity);

					for (ACardModel* Idx : Entity)
					{
						SacStructArray.Emplace(Idx -> CardStruct);
					}
					
					if (!URuleChecker::IsNecessarySacrificesForEffect(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, SacStructArray))
					{
						CheckHitResult() -> SetCardState(EState::KeepFocus);
						SacPickMap.Emplace(CheckHitResult() -> CardStruct.CardIndex, CheckHitResult());
					}
				}
				else
				{
					CheckHitResult() -> SetCardState(EState::Lerp);
					SacPickMap.Remove(CheckHitResult() -> CardStruct.CardIndex);
				}
				
				TArray<FCardStruct> SacStructArray;
				SacPickMap.GenerateValueArray(Entity);

				for (ACardModel* Idx : Entity)
				{
					SacStructArray.Emplace(Idx -> CardStruct);
				}
				
				if (URuleChecker::IsNecessarySacrificesForEffect(Cast<ACardCoreDriver>(GetWorld() ->  GetGameState()), PlayerState -> GetPlayerId(), FocusActor -> CardStruct, SacStructArray))	//满足度检查
				{
					MainUMG -> NotifyEndPickUpSacrifice(true);
				}
				else
				{
					MainUMG -> NotifyEndPickUpSacrifice(false);
				}
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
					RequestPlayCard(FocusActor->CardStruct.CardIndex, INT_ERROR, {});
					SetInputMode(EInputMode::Idle);
				}
				else
				{
					SetInputMode(EInputMode::PickUpSacrificesForPlay);
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
			if (CheckHitResult() && URuleChecker::CanAttack_Client(Cast<ACardCoreDriver>(GetWorld() -> GetGameState()), PlayerState -> GetPlayerId(),
				FocusActor -> CardStruct, CheckHitResult() -> CardStruct))
			{
				RequestAttack(FocusActor -> CardStruct.CardIndex, CheckHitResult() -> CardStruct.CardIndex);
				SetInputMode(EInputMode::Idle);	//Activate在再次按下后处理
			}
			break;
		}
	case EInputMode::Activate :
		{
			SetInputMode(EInputMode::PickUpSacrificesForEffect);	//若Activate在按下时修改，则必然发生！
			break;
		}
	case EInputMode::PickUpSacrificesForPlay :
		{
			//添加对象并检测是否完成
			break;
		}
	case EInputMode::PickUpSacrificesForEffect :
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
	TArray<int> SacIndexArray;
	TArray<ACardModel*> Entity;
	SacPickMap.GenerateKeyArray(SacIndexArray);
	SacPickMap.GenerateValueArray(Entity);
	SacPickMap.Empty();

	if (InputMode == EInputMode::PickUpSacrificesForPlay)
	{
		RequestPlayCard(FocusActor -> CardStruct.CardIndex, INT_ERROR, SacIndexArray);
		SetInputMode(EInputMode::Idle);
		return;
	}

	if (InputMode == EInputMode::PickUpSacrificesForEffect)	//分别筛选
	{
		RequestActivate(FocusActor -> CardStruct.CardIndex, SacIndexArray);
		SetInputMode(EInputMode::Idle);
		return;
	}
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
