#include "../CardCoreSystem/CardCoreDriver.h"
#include "../CardCoreSystem/EventPackage.h"
#include "../CardCoreSystem/EffectContext.h"
#include "../CardCoreSystem/EffectInstance.h"
#include "../CardCoreSystem/CardInstance.h"
#include "Algo/RandomShuffle.h"
#include "../Client/EventListener.h"
#include "../GamePlay/AIPlayer.h"
#include "../GamePlay/CardPlayer.h"
#include "Kismet/GameplayStatics.h"

//*****************************公共函数*********************************
UCardInstance* ACardCoreDriver::GetCardInstanceByIndex(const int CardIndex)	//基于卡牌ID寻找卡牌
{
	UCardInstance** CardInstance = MixDeck.FindByPredicate([CardIndex](const UCardInstance* Card) {return Card && Card->CardStruct.CardIndex == CardIndex;});
	return CardInstance ? *CardInstance : nullptr;
}

UCardInstance* ACardCoreDriver::GetCardInstanceByPlayerIndexAndZone(const int PlayerIndex, const EZone Zone)	//基于玩家ID与区域随机寻找卡牌
{
	UCardInstance** CardInstance;
	switch (Zone)
		{
		case EZone::DeckZone :
			CardInstance = InDeckZone.FindByPredicate([PlayerIndex](const UCardInstance* Card) {return Card && Card->CardStruct.PlayerIndex == PlayerIndex;});
			break;
		case EZone::HandZone :
			 CardInstance = InHandZone.FindByPredicate([PlayerIndex](const UCardInstance* Card) {return Card && Card->CardStruct.PlayerIndex == PlayerIndex;});
			break;
		case EZone::BoardZone :
			CardInstance = InBoardZone.FindByPredicate([PlayerIndex](const UCardInstance* Card) {return Card && Card->CardStruct.PlayerIndex == PlayerIndex;});
			break;
		case EZone::EchoZone : 
			CardInstance = InEchoZone.FindByPredicate([PlayerIndex](const UCardInstance* Card) {return Card && Card->CardStruct.PlayerIndex == PlayerIndex;});
			break;
		case EZone::GraveZone :
			CardInstance = InGraveZone.FindByPredicate([PlayerIndex](const UCardInstance* Card) {return Card && Card->CardStruct.PlayerIndex == PlayerIndex;});
			break;
			default: CardInstance = nullptr; break;
		}
		return CardInstance ? *CardInstance : nullptr;
	}

int ACardCoreDriver::GetZoneSizeByPlayerIndexAndZone(const int PlayerIndex, const EZone Zone)	//统计目标区域玩家卡牌数量
{
	const TArray<UCardInstance*>* FZone = nullptr;
	
	switch (Zone)
	{
	case EZone::DeckZone :
		FZone = &InDeckZone; break;
	case EZone::HandZone :
		FZone = &InHandZone; break;
	case EZone::BoardZone :
		FZone = &InBoardZone; break;
	case EZone::EchoZone : 
		FZone = &InEchoZone; break;
	case EZone::GraveZone :
		FZone = &InGraveZone; break;
	default : break;
	}
	if (!FZone) return INT_ERROR;
	
	int Num = 0; 
	for (const UCardInstance* CardInstance : *FZone)
	{
		if (CardInstance -> CardStruct.PlayerIndex == PlayerIndex)
		{
			Num++;
		}
	}
	return Num;
}

//**********************************************************************

//******************************效果栈**********************************

void ACardCoreDriver::CreateNewEffectForCondition(UCardInstance* EffectSource, UEffectContext* OuterContext)
{
	if (EffectSource)
	{
		const TSubclassOf<UEffectInstance>* EffectClassPtr = EffectSource -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) return;	//从时机到效果类的映射中寻找是否存在效果
		
		const TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) return;
		if (!CDO -> CheckTargets_Effect(this)) return;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = EffectSource;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);	//入栈
	}
}

void ACardCoreDriver::CreateNewEffectForConditionEliminate(UCardInstance* EffectSource, UEffectContext* OuterContext)
{
		//按战场、回响、手牌、墓地、卡组的顺序，检测其他卡牌是否有效果响应
	for (UCardInstance* CardInstance : InBoardZone)
	{
		if (CardInstance == EffectSource) continue;
		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardInstance -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) continue;	//从时机到效果类的映射中寻找是否存在效果
		
		TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) continue;
		if (!CDO -> CheckTargets_Effect(this)) continue;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = CardInstance;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);
	}

	for (UCardInstance* CardInstance : InEchoZone)
	{
		if (CardInstance == EffectSource) continue;
		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardInstance -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) continue;	//从时机到效果类的映射中寻找是否存在效果
		
		TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) continue;
		if (!CDO -> CheckTargets_Effect(this)) continue;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = CardInstance;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);
	}

	for (UCardInstance* CardInstance : InHandZone)
	{
		if (CardInstance == EffectSource) continue;
		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardInstance -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) continue;	//从时机到效果类的映射中寻找是否存在效果
		
		TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) continue;
		if (!CDO -> CheckTargets_Effect(this)) continue;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = CardInstance;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);
	}

	for (UCardInstance* CardInstance : InGraveZone)
	{
		if (CardInstance == EffectSource) continue;
		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardInstance -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) continue;	//从时机到效果类的映射中寻找是否存在效果
		
		TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) continue;
		if (!CDO -> CheckTargets_Effect(this)) continue;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = CardInstance;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);
	}

	for (UCardInstance* CardInstance : InDeckZone)
	{
		if (CardInstance == EffectSource) continue;
		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardInstance -> EffectForCondition.Find(OuterContext -> Condition);
		if (!EffectClassPtr) continue;	//从时机到效果类的映射中寻找是否存在效果
		
		TSubclassOf<UEffectInstance> EffectClass = *EffectClassPtr;
		UEffectInstance* CDO = EffectClass -> GetDefaultObject<UEffectInstance>();
		if (!CDO -> CheckCondition_Effect(OuterContext)) continue;
		if (!CDO -> CheckTargets_Effect(this)) continue;	//CDO检测时机是否正确，对象是否存在

		UEffectInstance* NewEffect = NewObject<UEffectInstance>(GetWorld(), EffectClass);	//创建效果实例
		NewEffect -> CardInstance = CardInstance;
		NewEffect -> StoredContext = OuterContext;
		NewEffect -> Driver = this;
		Stack.Emplace(NewEffect);
	}
}

void ACardCoreDriver::SolveStack()
{
	PendingEffect = nullptr;
	if (Stack.IsEmpty()) return;
	PendingEffect = Stack.Pop(EAllowShrinking::Yes);
	PendingEffect -> Activate();
}

void ACardCoreDriver::RequireForTarget_Implementation(const int PlayerIndex, const TArray<int>& ValidTargets)
{
	FEventPackageStruct Package;	//打包结构体并转发
	Package.PackageType = EPackageType::TargetPickUp;
	Package.Params.Emplace(FEventParamStruct::MakeIndexArray(ValidTargets));
	Package.FocusPlayerIndex.Emplace(PlayerIndex);
	DispatchEventPackageAsync(Package);
}

void ACardCoreDriver::CallBackForTarget(const int PickedTarget)
{
	if (PendingEffect)
	{
		PendingEffect -> CallBack(GetCardInstanceByIndex(PickedTarget));	//对待处理效果补充对象
	}
}
//**********************************************************************

//******************************卡牌生成********************************
void ACardCoreDriver::CallBackForDeck(const int PlayerIndex, const TArray<int>& CardID)
{
	if(Clients.Find(PlayerIndex))	return;		//重复添加
	Clients.Add(PlayerIndex, true);
	
	const UDataTable* CardInfoLib = LoadObject<UDataTable>(nullptr,TEXT("/Game/Library/CardInfoLib.CardInfoLib"));
	if (!CardInfoLib) return;
	
	for (const int Idx : CardID)	//遍历CardID创建卡牌
	{
		if (const FCardStruct* Row = CardInfoLib -> FindRow<FCardStruct>(FName(*FString::FromInt(Idx)), TEXT("Not Found")))
		{
			CreateNewCard(PlayerIndex, *Row);
		}
	}

	Algo::RandomShuffle(InDeckZone);
	Algo::RandomShuffle(InDeckZone);
	Algo::RandomShuffle(InDeckZone);	//连续洗牌三次
	
	if (Clients.Find(0) && Clients.Find(1))	//卡组准备就绪，可以开始。投硬币
	{
		TArray<EPhase> TollCoin = {EPhase::Player_0_Turn, EPhase::Player_1_Turn};
		SetGamePhase(TollCoin[FMath::RandBool()]);
	}
}

void ACardCoreDriver::CreateNewCard(const int PlayerIndex, const FCardStruct& OuterCardStruct)
{
	UCardInstance* NewCard = NewObject<UCardInstance>(GetWorld(), OuterCardStruct.CardInstanceClass);
	NewCard -> CardStruct = OuterCardStruct;
	NewCard -> CardStruct.PlayerIndex = PlayerIndex;
	NewCard -> CardStruct.CardIndex = MixDeck.Num() + 2;	//0和1视为双方玩家。卡牌与玩家主控角色本质是同构的。
	NewCard -> Driver = this;
	MixDeck.Emplace(NewCard);	//必然位于牌堆内
	InDeckZone.Emplace(NewCard);	//卡牌初始位于牌组内
}
//**********************************************************************

//******************************游戏状态********************************
void ACardCoreDriver::SetGamePhase(const EPhase NewPhase)
{
	if (GamePhase == NewPhase) return;
	
	switch (GamePhase)
	{
	case EPhase::GameStart : OnQuit_GameStart(); break;
	case EPhase::Player_0_Turn : OnQuit_Player_0_Turn(); break;
	case EPhase::Player_1_Turn : OnQuit_Player_1_Turn(); break;
	case EPhase::GameEnd : break;	
	}

	GamePhase = NewPhase;

	switch (GamePhase)
	{
	case EPhase::GameStart : break;
	case EPhase::Player_0_Turn : OnEnter_Player_0_Turn(); break;
	case EPhase::Player_1_Turn : OnEnter_Player_1_Turn(); break;
	case EPhase::GameEnd : OnEnter_GameEnd(); break;	
	}
}

void ACardCoreDriver::OnQuit_GameStart()
{
	FEventPackageStruct Package;	//打包结构体并转发
	Package.PackageType = EPackageType::GameStart;
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
	
	//初始阶段，玩家抽牌
	NormalDrawToCount(0,5);
	NormalDrawToCount(1,5);
}

void ACardCoreDriver::OnEnter_Player_0_Turn()
{
	TurnNum++;
	FEventPackageStruct Package;	//打包结构体并转发
	Package.PackageType = EPackageType::TurnStart;
	Package.Params.Emplace(FEventParamStruct::MakeInt(0));	
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);

	NormalDrawToCount(0,5);
}

void ACardCoreDriver::OnQuit_Player_0_Turn()
{
	//效果结算
	SetGamePhase(EPhase::Player_1_Turn);
}

void ACardCoreDriver::OnEnter_Player_1_Turn()
{
	TurnNum++;
	FEventPackageStruct Package;	//打包结构体并转发
	Package.PackageType = EPackageType::TurnStart;
	Package.Params.Emplace(FEventParamStruct::MakeInt(1));	
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
	
	NormalDrawToCount(1,5);
}

void ACardCoreDriver::OnQuit_Player_1_Turn()
{
	//效果结算
	SetGamePhase(EPhase::Player_0_Turn);
}

void ACardCoreDriver::OnEnter_GameEnd()
{
	FEventPackageStruct Package;	//打包结构体并转发
	Package.PackageType = EPackageType::GameEnd;
	Package.Params.Emplace(FEventParamStruct::MakeInt(WinnerIndex));	
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
}
//**********************************************************************

//******************************事件转发*******************************
void ACardCoreDriver::DispatchEventPackageSync_Implementation(const FEventPackageStruct& Package)
{
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		TArray<AActor*> Controllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AController::StaticClass(), Controllers);
		for (AActor* Idx : Controllers)
		{
			if (Cast<ACardPlayer>(Idx))
			{
				if (UEventListener* EventListener = Cast<ACardPlayer>(Idx) -> EventListener)
				{
					EventListener -> ListenEventPackageSync(Package);
					continue;
				}
			}
			if (Cast<AAIPlayer>(Idx))
			{
				if (UEventListener* EventListener = Cast<AAIPlayer>(Idx) -> EventListener)
				{
					EventListener -> ListenEventPackageSync(Package);
				}
			}
		}
	}
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		if (UEventListener* EventListener = Cast<ACardPlayer>(GetWorld() -> GetFirstPlayerController()) -> EventListener)
		{
			EventListener -> ListenEventPackageSync(Package);
		}
	}
}

void ACardCoreDriver::DispatchEventPackageAsync_Implementation(const FEventPackageStruct& Package)
{
	if (GetWorld()->GetNetMode() == NM_Standalone)
	{
		TArray<AActor*> Controllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AController::StaticClass(), Controllers);
		for (AActor* Idx : Controllers)
		{
			if (Cast<ACardPlayer>(Idx))
			{
				if (UEventListener* EventListener = Cast<ACardPlayer>(Idx) -> EventListener)
				{
					EventListener -> ListenEventPackageAsync(Package);
					continue;
				}
			}
			if (Cast<AAIPlayer>(Idx))
			{
				if (UEventListener* EventListener = Cast<AAIPlayer>(Idx) -> EventListener)
				{
					EventListener -> ListenEventPackageAsync(Package);
				}
			}
		}
	}
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		if (UEventListener* EventListener = Cast<ACardPlayer>(GetWorld() -> GetFirstPlayerController()) -> EventListener)
		{
			EventListener -> ListenEventPackageAsync(Package);
		}
	}
}
//**********************************************************************

//****************************原子事件系统*****************************
void ACardCoreDriver::CardMove(UCardInstance* CardInstance, EZone FromZone, EZone ToZone, EReason Reason)	//原子系统不验证，直接执行，负责表现
{
	if (!CardInstance || CardInstance -> CardStruct.CardZone != FromZone) return;
	CardInstance->CardStruct.CardZone = ToZone;

	switch (FromZone)
	{
	case EZone::DeckZone:	InDeckZone.Remove(CardInstance);		break;
	case EZone::HandZone:	InHandZone.Remove(CardInstance);		break;
	case EZone::BoardZone:	InBoardZone.Remove(CardInstance);		break;
	case EZone::EchoZone:	InEchoZone.Remove(CardInstance);		break;
	case EZone::GraveZone:	InGraveZone.Remove(CardInstance);		break;
	default: break;
	}
	
	switch (ToZone)
	{
	case EZone::DeckZone:	InDeckZone.Add(CardInstance);			break;
	case EZone::HandZone:	InHandZone.Add(CardInstance);		break;
	case EZone::BoardZone:	InBoardZone.Add(CardInstance);		break;
	case EZone::EchoZone:	InEchoZone.Add(CardInstance);			break;
	case EZone::GraveZone:	InGraveZone.Add(CardInstance);		break;
	default: break;
	}
	
	FEventPackageStruct Package;
	Package.PackageType = EPackageType::CardMove;
	Package.Params.Add(FEventParamStruct::MakeCardOrPlayer(CardInstance -> CardStruct));
	Package.Params.Add(FEventParamStruct::MakeZone(FromZone));
	Package.Params.Add(FEventParamStruct::MakeReason(Reason));
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
}

void ACardCoreDriver::CardAttach(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, EReason Reason)
{
	
}

void ACardCoreDriver::CardAttack(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, EReason Reason)
{
	if (!SourceCardInstance || !TargetCardInstance) return;
	const int ATK1 = SourceCardInstance -> CardStruct.Cur_AP;
	const int ATK2 = TargetCardInstance -> CardStruct.Cur_AP;
	CardApplyDamage(SourceCardInstance, TargetCardInstance, ATK1, EReason::PlaceHolder);
	CardApplyDamage(TargetCardInstance,SourceCardInstance, ATK2, EReason::PlaceHolder);

	FEventPackageStruct Package;
	Package.PackageType = EPackageType::CardAttack;
	Package.Params.Add(FEventParamStruct::MakeCardOrPlayer(SourceCardInstance -> CardStruct));
	Package.Params.Add(FEventParamStruct::MakeCardOrPlayer(SourceCardInstance -> CardStruct));
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
}

void ACardCoreDriver::CardApplyDamage(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, int Damage, EReason Reason)
{
	
}

void ACardCoreDriver::CardApplyHeal(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, int Heal, EReason Reason)
{
	
}

void ACardCoreDriver::CardActivate(UCardInstance* CardInstance, TArray<UCardInstance*> Sacrifice, EReason Reason)
{
	if (!CardInstance) return;
	
	FEventPackageStruct Package;
	Package.PackageType = EPackageType::CardActivate;
	Package.Params.Add(FEventParamStruct::MakeCardOrPlayer(CardInstance -> CardStruct));
	Package.GlobalEventIndex = GlobalEventIndex++;
	DispatchEventPackageSync(Package);
}

void ACardCoreDriver::CardUpdate(UCardInstance* CardInstance, EReason Reason)
{
	
}

void ACardCoreDriver::CardApplyEffect(UCardInstance* SourceCardInstance, UEffectInstance* TargetEffectInstance, EReason Reason)
{
	
}

void ACardCoreDriver::CardReveal(UCardInstance* CardInstance, EReason Reason)
{
	
}
//**********************************************************************

//*****************************一般性事件******************************
void ACardCoreDriver::NormalDraw(const int PlayerIndex, const int Num)
{
	for (int Idx = 0; Idx < Num; Idx++)
	{
		if (UCardInstance* CardInstance = GetCardInstanceByPlayerIndexAndZone(PlayerIndex, EZone::DeckZone))
		{
			CardMove(CardInstance, EZone::DeckZone, EZone::HandZone, EReason::Normally);
		}
	}
}

void ACardCoreDriver::NormalDrawToCount(const int PlayerIndex, const int MaxNum)
{
	int RealNum = 0;
	for (const UCardInstance* CardInstance : InHandZone)
	{
		if (CardInstance && CardInstance -> CardStruct.PlayerIndex == PlayerIndex)
		{
			RealNum ++;
		}
	}
	NormalDraw(PlayerIndex, FMath::Max(0, MaxNum - RealNum));
}
//**********************************************************************

//****************************玩家输入事件****************************
void ACardCoreDriver::ReceiveEndTurn(const int PlayerIndex)
{
	if (GamePhase == (PlayerIndex == 0 ? EPhase::Player_1_Turn : EPhase::Player_0_Turn)) return;	//请求与回合不匹配
	
	SetGamePhase( PlayerIndex == 0 ? EPhase::Player_1_Turn : EPhase::Player_0_Turn);
	
	SolveStack();
}

void ACardCoreDriver::ReceivePlayCard(const int PlayerIndex, const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)	//基于后进先出的顺序，先生成本身的事件，再检测代价
{
	if (false) return;
	
	switch(GetCardInstanceByIndex(SourceCard) -> CardStruct.CardType)
	{
		case EType::Servant : SummonServant(SourceCard, TargetCard, Sacrifice); break;
		case EType::Spell : CastSpell(SourceCard, TargetCard, Sacrifice); break;
		case EType::Terrain : BuildTerrain(SourceCard, TargetCard, Sacrifice); break;
		case EType::Equip :	AttachEquip(SourceCard, TargetCard, Sacrifice); break;
		default	: break;
	}

	for (const int Idx : Sacrifice)
	{
		PayCostAsSacrifice(Idx, SourceCard);
	}

	SolveStack();
}

void ACardCoreDriver::ReceiveAttack(const int PlayerIndex, const int SourceCard, const int TargetCard)
{
	if (false) return;

	CardAttack(GetCardInstanceByIndex(SourceCard), GetCardInstanceByIndex(TargetCard), EReason::PlaceHolder);

	SolveStack();
}

void ACardCoreDriver::ReceiveActivate(const int PlayerIndex, const int SourceCard, const TArray<int>& Sacrifice)	//这些效果遵循后进先出。
{
	if (false) return;
	
	TArray<UCardInstance*> SacrificeCardInstances = {};
	for (const int Idx : Sacrifice)
	{
		SacrificeCardInstances.Emplace(GetCardInstanceByIndex(Idx));
	}

	CardActivate(GetCardInstanceByIndex(SourceCard), SacrificeCardInstances, EReason::PlaceHolder);
	
	UEffectContextForOnActivate* MulticastContext = NewObject<UEffectContextForOnActivate>(GetWorld());
	MulticastContext -> Condition = ECondition::OnActivate;
	MulticastContext -> SourceCard = GetCardInstanceByIndex(SourceCard);
	CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);

	UEffectContextForActivate* NewContext = NewObject<UEffectContextForActivate>(GetWorld());
	NewContext -> Condition = ECondition::Activate;
	NewContext -> Sacrifice = SacrificeCardInstances;
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
	
	for (const int Idx : Sacrifice)
	{
		PayCostAsSacrifice(Idx, SourceCard);
	}

	SolveStack();
}

void ACardCoreDriver::ReceiveTarget(const int PlayerIndex, const int PickedTarget)
{
	if (false) return;

	CallBackForTarget(PickedTarget);
}
//**********************************************************************


//*****************************解释性事件*******************************
void ACardCoreDriver::SummonServant(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;
	
	CardMove(GetCardInstanceByIndex(SourceCard), EZone::HandZone, EZone::BoardZone, EReason::Normally);
	
	UEffectContextForOnSummonServant* MulticastContext = NewObject<UEffectContextForOnSummonServant>(GetWorld());
	MulticastContext -> Condition = ECondition::OnSummonServant;
	MulticastContext -> SourceCard = GetCardInstanceByIndex(SourceCard);
	CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);
	
	UEffectContextForBattleCry* NewContext = NewObject<UEffectContextForBattleCry>(GetWorld());
	NewContext -> Condition = ECondition::BattleCry;
	TArray<UCardInstance*> SacrificeCardInstances = {};
	for (const int Idx : Sacrifice)
	{
		SacrificeCardInstances.Emplace(GetCardInstanceByIndex(Idx));
	}
	NewContext ->  PrePickUp = GetCardInstanceByIndex(TargetCard);
	NewContext -> Sacrifice = SacrificeCardInstances;
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
}

void ACardCoreDriver::CastSpell(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;
	
	TryMoveToEcho(SourceCard, TargetCard);
	
	UEffectContextForOnCastSpell* MulticastContext = NewObject<UEffectContextForOnCastSpell>(GetWorld());
	MulticastContext -> Condition = ECondition::OnCastSpell;
	MulticastContext -> SourceCard = GetCardInstanceByIndex(SourceCard);
	CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);
	
	UEffectContextForCastSpell* NewContext = NewObject<UEffectContextForCastSpell>(GetWorld());
	NewContext -> Condition = ECondition::CastSpell;
	TArray<UCardInstance*> SacrificeCardInstances = {};
	for (const int Idx : Sacrifice)
	{
		SacrificeCardInstances.Emplace(GetCardInstanceByIndex(Idx));
	}
	NewContext ->  PrePickUp = GetCardInstanceByIndex(TargetCard);
	NewContext -> Sacrifice = SacrificeCardInstances;
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
}

void ACardCoreDriver::BuildTerrain(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;
	
	CardMove(GetCardInstanceByIndex(SourceCard), EZone::HandZone, EZone::BoardZone, EReason::Normally);
		
	UEffectContextForOnBuildTerrain* MulticastContext = NewObject<UEffectContextForOnBuildTerrain>(GetWorld());
	MulticastContext -> Condition = ECondition::OnBuildTerrain;
	MulticastContext -> SourceCard = GetCardInstanceByIndex(SourceCard);
	CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);
	
	UEffectContextForBattleCry* NewContext = NewObject<UEffectContextForBattleCry>(GetWorld());
	NewContext -> Condition = ECondition::BattleCry;
	TArray<UCardInstance*> SacrificeCardInstances = {};
	for (const int Idx : Sacrifice)
	{
		SacrificeCardInstances.Emplace(GetCardInstanceByIndex(Idx));
	}
	NewContext ->  PrePickUp = GetCardInstanceByIndex(TargetCard);
	NewContext -> Sacrifice = SacrificeCardInstances;
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
}

void ACardCoreDriver::AttachEquip(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;
	
	CardMove(GetCardInstanceByIndex(SourceCard), EZone::HandZone, EZone::BoardZone, EReason::Normally);
	CardAttach(GetCardInstanceByIndex(SourceCard), GetCardInstanceByIndex(TargetCard), EReason::PlaceHolder);

	UEffectContextForOnAttachEquip* MulticastContext = NewObject<UEffectContextForOnAttachEquip>(GetWorld());
	MulticastContext -> Condition = ECondition::OnAttachEquip;
	MulticastContext -> SourceCard = GetCardInstanceByIndex(SourceCard);
	CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);
	
	UEffectContextForBattleCry* NewContext = NewObject<UEffectContextForBattleCry>(GetWorld());
	NewContext -> Condition = ECondition::BattleCry;
	TArray<UCardInstance*> SacrificeCardInstances = {};
	for (const int Idx : Sacrifice)
	{
		SacrificeCardInstances.Emplace(GetCardInstanceByIndex(Idx));
	}
	NewContext ->  PrePickUp = GetCardInstanceByIndex(TargetCard);
	NewContext -> Sacrifice = SacrificeCardInstances;
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
}

void ACardCoreDriver::PayCostAsSacrifice(const int SourceCard, const int RelativeCard)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;
	if (!GetCardInstanceByIndex(RelativeCard)) return;
	
	CardMove(GetCardInstanceByIndex(SourceCard), GetCardInstanceByIndex(SourceCard) -> CardStruct.CardZone, EZone::GraveZone, EReason::Sacrifice);

	UEffectContextForOnSacrificed* NewContext = NewObject<UEffectContextForOnSacrificed>(GetWorld());
	NewContext -> Condition = ECondition::OnSacrificed;
	NewContext ->  RelativeCard = GetCardInstanceByIndex(RelativeCard);
	CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
}

void ACardCoreDriver::TryMoveToEcho(const int SourceCard, const int RelativeCard)
{
	if (!GetCardInstanceByIndex(SourceCard)) return;

	if (GetZoneSizeByPlayerIndexAndZone(GetCardInstanceByIndex(SourceCard) -> CardStruct.PlayerIndex, EZone::EchoZone) != INT_ERROR &&
		GetZoneSizeByPlayerIndexAndZone(GetCardInstanceByIndex(SourceCard) -> CardStruct.PlayerIndex, EZone::EchoZone) < 6)	//回响区仍有空余
	{
		CardMove(GetCardInstanceByIndex(SourceCard), GetCardInstanceByIndex(SourceCard) -> CardStruct.CardZone, EZone::EchoZone, EReason::Normally);

		UEffectContextForOnDeathRattle* MulticastContext = NewObject<UEffectContextForOnDeathRattle>(GetWorld());
		MulticastContext -> Condition = ECondition::OnDeathRattle;
		MulticastContext ->  SourceCard = GetCardInstanceByIndex(SourceCard);
		MulticastContext ->  RelativeCard = GetCardInstanceByIndex(RelativeCard);
		CreateNewEffectForConditionEliminate(GetCardInstanceByIndex(SourceCard), MulticastContext);
		
		UEffectContextForDeathRattle* NewContext = NewObject<UEffectContextForDeathRattle>(GetWorld());
		NewContext -> Condition = ECondition::DeathRattle;
		NewContext ->  RelativeCard = GetCardInstanceByIndex(RelativeCard);
		CreateNewEffectForCondition(GetCardInstanceByIndex(SourceCard), NewContext);
	}
	else
	{
		CardMove(GetCardInstanceByIndex(SourceCard), GetCardInstanceByIndex(SourceCard) -> CardStruct.CardZone, EZone::GraveZone, EReason::Normally);
	}
}

//**********************************************************************
