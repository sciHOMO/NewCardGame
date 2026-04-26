#pragma once

#include "CoreMinimal.h"
#include "CardInstance.h"
#include "GameFramework/GameState.h"
#include "CardCoreDriver.generated.h"

enum class EReason : uint8;
enum class EZone : uint8;
struct FCardStruct;
struct FEffectContext;
struct FEventPackageStruct;
class UCardInstance;
class UEffectInstance;
class UEffectContext;

/*
 卡牌效果核心驱动器，本质是AGameState
*/

//时机枚举型
UENUM(BlueprintType)
enum class EPhase : uint8
{
	GameStart				UMETA(DisplayName = "GameStart"),
	Player_0_Turn			UMETA(DisplayName = "Player_0_Turn"),
	Player_1_Turn			UMETA(DisplayName = "Player_1_Turn"),
	GameEnd				UMETA(DisplayName = "GameEnd"),
};

UCLASS()
class ACardCoreDriver : public AGameState
{
	GENERATED_BODY()

public:
	
	//****************************公共函数********************************
	UFUNCTION()
	UCardInstance* GetCardInstanceByIndex(const int CardIndex);	//这些公共函数用来获取、统计游戏逻辑相关的实例

	UFUNCTION()
	UCardInstance* GetCardInstanceByPlayerIndexAndZone(const int PlayerIndex, const EZone Zone);

	UFUNCTION()
	int GetZoneSizeByPlayerIndexAndZone(const int PlayerIndex, const EZone Zone);
	//********************************************************************
	
	//*****************************效果栈*********************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UEffectInstance*> Stack = {};	//效果栈

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UEffectInstance* PendingEffect = nullptr;	//待处理对象
	
	UFUNCTION()
	void CreateNewEffectForCondition(UCardInstance* EffectSource, UEffectContext* OuterContext);	//询问对象的效果

	UFUNCTION()
	void CreateNewEffectForConditionEliminate(UCardInstance* EffectSource, UEffectContext* OuterContext);	//排除对象自身后询问相关效果，群发遍历
	
	UFUNCTION()
	void SolveStack();	//出栈

	UFUNCTION(NetMulticast, Reliable)
	void RequireForTarget(const int PlayerIndex, const TArray<int>& ValidTargets);	//网络广播，请求合法对象

	UFUNCTION()
	void CallBackForTarget(const int PickedTarget);	//回调，补充合法对象
	//********************************************************************

	//*****************************卡牌生成*******************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> MixDeck;	//原始共享牌堆

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> InDeckZone;	//牌组

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> InHandZone;	//手牌

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> InBoardZone;	//战场

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> InEchoZone;	//回响

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> InGraveZone;	//墓地

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TMap <int, bool> Clients;	//客户端就绪Map
	
	UFUNCTION()
	void CallBackForDeck(const int PlayerIndex, const TArray<int>& CardID);	//回调接收牌组序列
	
	UFUNCTION()
	void CreateNewCard(const int PlayerIndex, const FCardStruct& OuterCardStruct);	//生成卡牌
	//********************************************************************

	//*****************************游戏状态*******************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EPhase GamePhase = EPhase::GameStart;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int WinnerIndex = INT_ERROR;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int TurnNum = 0;
	
	UFUNCTION()
	void SetGamePhase(const EPhase NewPhase);	//伪状态机

	void OnQuit_GameStart();
	void OnEnter_Player_0_Turn();
	void OnQuit_Player_0_Turn();
	void OnEnter_Player_1_Turn();
	void OnQuit_Player_1_Turn();
	void OnEnter_GameEnd();
	//********************************************************************

	//*****************************事件转发*******************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int64 GlobalEventIndex = 0;	//全局事件ID
	
	UFUNCTION(NetMulticast, Reliable)
	void DispatchEventPackageSync(const FEventPackageStruct& Package);	//预计仅用于事件，线性处理

	UFUNCTION(NetMulticast, Reliable)
	void DispatchEventPackageAsync(const FEventPackageStruct& Package);	//预计仅用于选择对象，异步
	//********************************************************************

	//****************************原子事件系统*****************************
	UFUNCTION(BlueprintCallable)
	void CardMove(UCardInstance* CardInstance, EZone FromZone, EZone ToZone,  EReason Reason);	//游戏逻辑分解至底层的原子事件

	UFUNCTION(BlueprintCallable)
	void CardAttach(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, EReason Reason);
	
	UFUNCTION(BlueprintCallable)
	void CardAttack(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, EReason Reason);

	UFUNCTION(BlueprintCallable)
	void CardApplyDamage(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, int Damage, EReason Reason);

	UFUNCTION(BlueprintCallable)
	void CardApplyHeal(UCardInstance* SourceCardInstance, UCardInstance* TargetCardInstance, int Heal, EReason Reason);
	
	UFUNCTION(BlueprintCallable)
	void CardActivate(UCardInstance* CardInstance, TArray<UCardInstance*> Sacrifice, EReason Reason);

	UFUNCTION(BlueprintCallable)
	void CardUpdate(UCardInstance* CardInstance, EReason Reason);

	UFUNCTION(BlueprintCallable)
	void CardApplyEffect(UCardInstance* SourceCardInstance, UEffectInstance* TargetEffectInstance, EReason Reason);
	
	UFUNCTION(BlueprintCallable)
	void CardReveal(UCardInstance* CardInstance, EReason Reason);
	//********************************************************************

	//****************************一般性事件*****************************
	UFUNCTION(BlueprintCallable)
	void NormalDraw(const int PlayerIndex, const int Num);

	UFUNCTION(BlueprintCallable)
	void NormalDrawToCount(const int PlayerIndex, const int MaxNum);
	//********************************************************************

	//****************************玩家输入事件****************************
	UFUNCTION()
	void ReceiveEndTurn(const int PlayerIndex);
	
	UFUNCTION()
	void ReceivePlayCard(const int PlayerIndex, const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);
	
	UFUNCTION()
	void ReceiveAttack(const int PlayerIndex, const int SourceCard, const int TargetCard);

	UFUNCTION()
	void ReceiveActivate(const int PlayerIndex, const int SourceCard, const TArray<int>& Sacrifice);

	UFUNCTION()
	void ReceiveTarget(const int PlayerIndex, const int PickedTarget);
	//**********************************************************************
	
	//*****************************解释性事件*******************************
	UFUNCTION()
	void SummonServant(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);

	UFUNCTION()
	void CastSpell(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);
	
	UFUNCTION()
	void BuildTerrain(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);
	
	UFUNCTION()
	void AttachEquip(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);

	UFUNCTION()
	void PayCostAsSacrifice(const int SourceCard, const int RelativeCard);

	UFUNCTION()
	void TryMoveToEcho(const int SourceCard, const int RelativeCard);
	//********************************************************************

	
};

