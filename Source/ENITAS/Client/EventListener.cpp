#include "../Client/EventListener.h"
#include "../CardCoreSystem/EventPackage.h"
#include "AIController.h"
#include "CardModel.h"
#include "MainUMG.h"
#include "ENITAS/GamePlay/CardPlayer.h"
#include "Kismet/GameplayStatics.h"

UEventListener::UEventListener()
{

}

//****************************本地效果接收********************************
void UEventListener::ListenEventPackageSync(const FEventPackageStruct& Package)
{
	Enqueue(Package);
}

void UEventListener::ListenEventPackageAsync(const FEventPackageStruct& Package)
{
	
}

void UEventListener::Enqueue(const FEventPackageStruct& Package)
{
	if (Package.GlobalEventIndex < LocalEventIndex) return;	//去重、排序
	if (PendingQueue.ContainsByPredicate([&](const FEventPackageStruct& FoundPackage) { return FoundPackage.GlobalEventIndex == Package.GlobalEventIndex; })) return;
	const int32 Index = Algo::LowerBound(PendingQueue, Package.GlobalEventIndex,[](const FEventPackageStruct& FoundPackage, const int64 Value) { return FoundPackage.GlobalEventIndex < Value;});
	PendingQueue.Insert(Package, Index);	//二分法插入
}

void UEventListener::CheckQueue()
{
	if (!PendingQueue.IsEmpty() && !ProcessingPackage.IsValid() && FindPackageByIndex(LocalEventIndex) && FindPackageByIndex(LocalEventIndex) -> IsValid() && !Cast<AAIController>(GetOwner()))	//不为机器人生成效果
	{
		ProcessingPackage = *FindPackageByIndex(LocalEventIndex);
		Execute(ProcessingPackage);
	}
}

void UEventListener::Execute(const FEventPackageStruct& Package)
{
	switch (Package.PackageType)
	{
		case EPackageType::GameStart:					HandleGameStart(Package);					break;
		case EPackageType::GameEnd:					HandleGameEnd(Package);					break;
		case EPackageType::TurnStart:					HandleTurnStart(Package);						break;
		case EPackageType::TurnEnd:						HandleTurnEnd(Package);						break;
		case EPackageType::CardMove:					HandleCardMove(Package);					break;
		case EPackageType::CardAttach:				HandleCardAttach(Package);					break;
		case EPackageType::CardAttack:					HandleCardAttack(Package);					break;
		case EPackageType::CardApplyDamage:	HandleCardApplyDamage(Package);		break;
		case EPackageType::CardActivate:				HandleCardActivate(Package);				break;
		case EPackageType::CardUpdate:				HandleCardUpdate(Package);				break;
		case EPackageType::CardApplyEffect:		HandleCardApplyEffect(Package);			break;
		case EPackageType::CardReveal:				HandleCardReveal(Package);					break;
		
		default : break;
	}
}

const FEventPackageStruct* UEventListener::FindPackageByIndex(int Index) const
{
	return PendingQueue.FindByPredicate([Index](const FEventPackageStruct& Package) {return Package.IsValid() && Package.GlobalEventIndex == Index;});
}

void UEventListener::Clear(int Index)
{
	check(LocalEventIndex == Index);	//确保回调与LocalEventIndex完全一致
	LocalEventIndex++;
	ProcessedQueue.Emplace(ProcessingPackage);
	PendingQueue.Remove(ProcessingPackage);
	PendingQueue.Shrink();
	ProcessingPackage = FEventPackageStruct();
}
//*************************************************************************

//****************************本地效果执行********************************
void UEventListener::HandleGameStart(const FEventPackageStruct& Package)
{
	Cast<ACardPlayer>(Controller) -> MainUMG -> NotifyGameStart();
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Index = Package.GlobalEventIndex](){this -> Clear(Index);});
	GetWorld() -> GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0F, false);
}

void UEventListener::HandleGameEnd(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleTurnStart(const FEventPackageStruct& Package)
{
	Cast<ACardPlayer>(Controller) -> MainUMG -> NotifyTurnStart(Package.Params[0].IntValue);
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, Index = Package.GlobalEventIndex](){this -> Clear(Index);});
	GetWorld() -> GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 1.0F, false);
}

void UEventListener::HandleTurnEnd(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardMove(const FEventPackageStruct& Package)
{
	if (Package.Params[0].CardOrPlayer.CardZone == EZone::HandZone && Package.Params[1].ZoneValue == EZone::DeckZone)
	{
		DrawCard(Package); return;
	}
	if (Package.Params[0].CardOrPlayer.CardZone == EZone::BoardZone && Package.Params[0].CardOrPlayer.CardType == EType::Servant &&
		Package.Params[1].ZoneValue == EZone::HandZone)
	{
		SummonServant(Package); return;
	}
	if ((Package.Params[0].CardOrPlayer.CardZone == EZone::EchoZone || Package.Params[0].CardOrPlayer.CardZone == EZone::GraveZone)
		&& Package.Params[0].CardOrPlayer.CardType == EType::Spell && Package.Params[1].ZoneValue == EZone::HandZone)
	{
		CastSpell(Package); return;
	}
}

void UEventListener::HandleCardAttach(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardAttack(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardApplyDamage(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardActivate(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardUpdate(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardApplyEffect(const FEventPackageStruct& Package)
{
	
}

void UEventListener::HandleCardReveal(const FEventPackageStruct& Package)
{

	
}
//****************************本地效果管理*********************************
void UEventListener::DrawCard(const FEventPackageStruct& Package)
{
	ACardModel* CardModel = CreateCardModel(Package.Params[0].CardOrPlayer);
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartDrawCard(Owning, Package);
}

void UEventListener::SummonServant(const FEventPackageStruct& Package)
{
	ACardModel* CardModel =FindCardModel(Package.Params[0].CardOrPlayer.CardIndex);
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartSummonServant(Owning, Package);
}

void UEventListener::CastSpell(const FEventPackageStruct& Package)
{
	ACardModel* CardModel =FindCardModel(Package.Params[0].CardOrPlayer.CardIndex);
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartCastSpell(Owning, Package);
}

ACardModel* UEventListener::CreateCardModel(const FCardStruct& CardStruct)
{
	//确定生成位置
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(FVector::ZeroVector);
	SpawnTransform.SetRotation(FRotator::ZeroRotator.Quaternion());
	SpawnTransform.SetScale3D(FVector::OneVector);

	//生成卡牌
	ACardModel* NewCard = GetWorld() -> SpawnActor<ACardModel>(CardStruct.CardModelClass, SpawnTransform);
	NewCard -> CardStruct = CardStruct;
	NewCard -> EventListener = this;
	
	return NewCard ? NewCard : nullptr;
}

ACardModel* UEventListener::FindCardModel(const int CardIndex)
{
	TArray<AActor*> TotalActors;
	UGameplayStatics::GetAllActorsOfClass(Controller -> GetWorld(), ACardModel::StaticClass(), TotalActors);
	TArray<ACardModel*> TotalCards;
	for (AActor* Idx : TotalActors)
	{
		TotalCards.Emplace(Cast<ACardModel>(Idx));
	}
	return *TotalCards.FindByPredicate([CardIndex](const ACardModel* CardModel) {return CardModel && CardModel -> CardStruct.CardIndex == CardIndex;});
}
//*************************************************************************

//****************************分发卡牌更新********************************
void UEventListener::RefreshHandZone(const int PlayerIndex)
{
	auto GetY = [](const int Count) -> float
	{
		if (Count <= 2) return 66.F;
		if (Count >= 10) return 30.F;
		return 66.F + (30.f - 66.F) * (Count - 2) / 8;
	};

	if (Controller->PlayerState->GetPlayerId() == PlayerIndex)
	{
		const int Count = OwnerHandZone.Num();
		const float YSpacing = GetY(Count);
		
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			ACardModel* CardModel = OwnerHandZone[Idx];
			const float Y = 350.F - (Count - 1) * 30.f + Idx * YSpacing;
			const float Z = 200.F + Idx * 5.0F;
			CardModel -> TargetLocation = FVector(-400.F, Y, Z);
		}
	}
	else
	{
		const int Count = EnemyHandZone.Num();
		const float YSpacing = GetY(Count);
		
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			ACardModel* CardModel = EnemyHandZone[Idx];
			const float Y = -350.F + (Count - 1) * 30.f - Idx * YSpacing;
			const float Z = 200.F + Idx * 5.0F;
			CardModel -> TargetLocation = FVector(500.f, Y, Z);
		}
	}
}

void UEventListener::RemoveFromHandZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerHandZone.Remove(CardModel);
	}
	else
	{
		EnemyHandZone.Remove(CardModel);
	}
}

void UEventListener::AddToHandZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerHandZone.Emplace(CardModel);
	}
	else
	{
		EnemyHandZone.Emplace(CardModel);
	}
}

void UEventListener::RefreshBoardZone(const int PlayerIndex)
{
	if (Controller -> PlayerState -> GetPlayerId() == PlayerIndex)
	{
		const int Count = OwnerBoardZone.Num();
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			ACardModel* CardModel = OwnerBoardZone[Idx];
			const float Y = -(Count - 1) * 100.F + Idx * 200.F;
			CardModel -> TargetLocation = FVector(-120.F, Y, 180.F);
		}
	}
	else
	{
		const int Count = EnemyBoardZone.Num();
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			ACardModel* CardModel = EnemyBoardZone[Idx];
			const float Y = (Count - 1) * 100.F - Idx * 200.F;
			CardModel -> TargetLocation = FVector(120.F, Y, 180.F);
		}
	}
}

void UEventListener::RemoveFromBoardZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerBoardZone.Remove(CardModel);
	}
	else
	{
		EnemyBoardZone.Remove(CardModel);
	}
}

void UEventListener::AddToBoardZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerBoardZone.Emplace(CardModel);
	}
	else
	{
		EnemyBoardZone.Emplace(CardModel);
	}
}

void UEventListener::RefreshEchoZone(const int PlayerIndex)
{
	auto GetY = [](const int Count) -> float
	{
		if (Count <= 2) return 66.F;
		if (Count >= 6) return 30.F;
		return 66.F + (30.F - 66.F) * (Count - 2) / 4;
	};
	
	if (Controller -> PlayerState -> GetPlayerId() == PlayerIndex)
	{
		const int Count = OwnerEchoZone.Num();
		const float YSpacing = GetY(Count);
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			ACardModel* CardModel = OwnerEchoZone[Idx];
			const float Y = -350.F + (Count - 1) * 30.F - Idx * YSpacing;
			const float Z = 180.F + Idx * 5.0F;
			CardModel -> TargetLocation = FVector(-400.F, Y, Z);
		}
	}
	else
	{
		const int Count = EnemyEchoZone.Num();
		for (int Idx = 0; Idx < Count; ++Idx)
		{
			const float YSpacing = GetY(Count);
			ACardModel* CardModel = EnemyEchoZone[Idx];
			const float Y = 350.F - (Count - 1) * 30.F + Idx * YSpacing;
			const float Z = 180.F + Idx * 5.0F;
			CardModel -> TargetLocation = FVector(500.F, Y, Z);
		}
	}
}

void UEventListener::RemoveFromEchoZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerEchoZone.Remove(CardModel);
	}
	else
	{
		EnemyHandZone.Remove(CardModel);
	}
}

void UEventListener::AddToEchoZone(ACardModel* CardModel)
{
	if (bool Owning = CardModel -> CardStruct.PlayerIndex == Controller -> PlayerState -> GetPlayerId())
	{
		OwnerEchoZone.Emplace(CardModel);
	}
	else
	{
		EnemyEchoZone.Emplace(CardModel);
	}
}
//*************************************************************************
