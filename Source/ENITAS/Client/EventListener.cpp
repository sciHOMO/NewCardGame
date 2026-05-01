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
	
	const EZone FromZone = Package.Params[1].ZoneValue;      // 从哪里来
	const EZone ToZone = Package.Params[0].CardOrPlayer.CardZone;  // 到哪里去
	
	if (FromZone == EZone::DeckZone && ToZone == EZone::HandZone && Package.Params[2].Reason == EReason::Normally)
	{
		DrawCard(Package); return;
	}
	
	if (FromZone == EZone::HandZone && ToZone == EZone::BoardZone && 
		Package.Params[0].CardOrPlayer.CardType == EType::Servant && Package.Params[2].Reason == EReason::Normally)
	{
		SummonServant(Package); return;
	}
	
	if (FromZone == EZone::HandZone && (ToZone == EZone::EchoZone || ToZone == EZone::GraveZone)
		&& Package.Params[0].CardOrPlayer.CardType == EType::Spell && Package.Params[2].Reason == EReason::Normally)
	{
		CastSpell(Package); return;
	}

	if (ToZone == EZone::GraveZone && Package.Params[2].Reason == EReason::Sacrifice)
	{
		Disappear(Package); return;
	}
	
	Clear(Package.GlobalEventIndex);
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
void UEventListener::GetCardsByZone(EZone Zone, int PlayerIndex, TArray<ACardModel*>& OutCards) const
{
	OutCards.Empty();
	for (ACardModel* Card : AllCardModels)
	{
		if (Card && Card->CardStruct.CardZone == Zone && Card->CardStruct.PlayerIndex == PlayerIndex)
		{
			OutCards.Add(Card);
		}
	}
}

void UEventListener::RefreshZone(EZone Zone, int PlayerIndex)
{
	TArray<ACardModel*> Cards;
    GetCardsByZone(Zone, PlayerIndex, Cards);
	const bool IsOwner = Controller->PlayerState->GetPlayerId() == PlayerIndex;
	const int TCount = Cards.Num();

    // 根据Zone类型应用不同的布局规则
    switch(Zone)
    {
        case EZone::HandZone:
        {
            auto GetY = [](const int Count) -> float
            {
                if (Count <= 2) return 66.F;
                if (Count >= 10) return 30.F;
                return 66.F + (30.f - 66.F) * (Count - 2) / 8;
            };

            const float YSpacing = GetY(TCount);

            for (int Idx = 0; Idx < TCount; ++Idx)
            {
                ACardModel* Card = Cards[Idx];
                const float Y = IsOwner 
                    ? (350.F - (TCount - 1) * 30.f + Idx * YSpacing)
                    : (-350.F + (TCount - 1) * 30.f - Idx * YSpacing);
                const float Z = 200.F + Idx * 5.0F;
                const float X = IsOwner ? -400.F : 500.f;

                Card -> TargetLocation = FVector(X, Y, Z);
            }
            break;
        }
        case EZone::BoardZone:
        {
            for (int Idx = 0; Idx < TCount; ++Idx)
            {
                ACardModel* Card = Cards[Idx];
                const float Y = IsOwner 
                    ? (-(TCount - 1) * 100.F + Idx * 200.F)
                    : ((TCount - 1) * 100.F - Idx * 200.F);
                const float X = IsOwner ? -120.F : 120.F;

                Card -> TargetLocation = FVector(X, Y, 180.F);
            }
            break;
        }
        case EZone::EchoZone:
        {
            auto GetY = [](const int Count) -> float
            {
                if (Count <= 2) return 66.F;
                if (Count >= 6) return 30.F;
                return 66.F + (30.F - 66.F) * (Count - 2) / 4;
            };

            const float YSpacing = GetY(TCount);

            for (int Idx = 0; Idx < TCount; ++Idx)
            {
                ACardModel* Card = Cards[Idx];
                const float Y = IsOwner
                    ? (-350.F + (TCount - 1) * 30.F - Idx * YSpacing)
                    : (350.F - (TCount - 1) * 30.F + Idx * YSpacing);
                const float Z = 180.F + Idx * 5.0F;
                const float X = IsOwner ? -400.F : 500.F;

                Card -> TargetLocation = FVector(X, Y, Z);
            }
            break;
        }
        case EZone::GraveZone:
        {
            RefreshZone(EZone::HandZone, PlayerIndex);
        	RefreshZone(EZone::EchoZone, PlayerIndex);
            break;
        }
        default: break;
    }
}

void UEventListener::AddCardToZone(ACardModel* CardModel, EZone NewZone)
{
	if (!CardModel) return;
	CardModel -> CardStruct.CardZone = NewZone;
	AllCardModels.AddUnique(CardModel);
	RefreshZone(NewZone, CardModel -> CardStruct.PlayerIndex);
}

void UEventListener::RemoveCardFromZone(ACardModel* CardModel)
{
	if (!CardModel) return;
	const int PlayerIndex = CardModel -> CardStruct.PlayerIndex;
	const EZone OldZone = CardModel -> CardStruct.CardZone;
	AllCardModels.Remove(CardModel);
	RefreshZone(OldZone, PlayerIndex);
}

void UEventListener::DrawCard(const FEventPackageStruct& Package)
{
	ACardModel* CardModel = CreateCardModel(Package.Params[0].CardOrPlayer);
	if (!CardModel) return;
	AllCardModels.Emplace(CardModel);
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartDrawCard(Owning, Package);
}

void UEventListener::SummonServant(const FEventPackageStruct& Package)
{
	ACardModel* CardModel =FindCardModel(Package.Params[0].CardOrPlayer.CardIndex);
	if (!CardModel) return;
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartSummonServant(Owning, Package);
}

void UEventListener::CastSpell(const FEventPackageStruct& Package)
{
	ACardModel* CardModel = FindCardModel(Package.Params[0].CardOrPlayer.CardIndex);
	if (!CardModel) return;
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartCastSpell(Owning, Package);
}

void UEventListener::Disappear(const FEventPackageStruct& Package)
{
	ACardModel* CardModel = FindCardModel(Package.Params[0].CardOrPlayer.CardIndex);
	if (!CardModel) return;
	const bool Owning = Package.Params[0].CardOrPlayer.PlayerIndex == Controller -> PlayerState -> GetPlayerId();
	CardModel -> StartDisappear(Owning, Package);
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
	
	return NewCard;
}

ACardModel* UEventListener::FindCardModel(const int CardIndex)
{
	
	for (ACardModel* Card : AllCardModels)
	{
		if (Card && Card->CardStruct.CardIndex == CardIndex)
		{
			return Card;
		}
	}
	return nullptr;
}
//*************************************************************************
