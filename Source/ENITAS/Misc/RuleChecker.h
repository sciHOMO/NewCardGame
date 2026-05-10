#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "../CardCoreSystem/CardInstance.h"
#include "../CardCoreSystem/EventPackage.h"
#include "RuleChecker.generated.h"

class ACardCoreDriver;
class UCardInstance;
class UEffectInstance;

/**
 * 服务器权威规则检测（蓝图可调用）。与 ACardCoreDriver 的玩家输入、原子事件、解释性事件对齐。
 */

UCLASS()
class URuleChecker : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	//****************************玩家输入预检测*******************************
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool CanEndTurn_Client(const ACardCoreDriver* Driver, const int PlayerIndex);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool CanPlayCard_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool CanAttackOrActivate_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct);
	
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool CanAttack_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& AttackerStruct, const FCardStruct& DefenderStruct);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool CanActivate_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool IsValidSacrificeForPlay(const ACardCoreDriver* Driver, const int PlayerIndex,const FCardStruct& CardStruct, const FCardStruct& SacrificeStruct);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool IsValidSacrificeForEffect(const ACardCoreDriver* Driver, const int PlayerIndex,const FCardStruct& CardStruct, const FCardStruct& SacrificeStruct);
	
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool IsNecessarySacrificesForPlay(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Client")
	static bool IsNecessarySacrificesForEffect(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice);
	//***************************************************************
	
	//*************************服务器校验****************************
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Server")
	static bool CanEndTurn_Server(const ACardCoreDriver* Driver, const int PlayerIndex);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Server")
	static bool CanPlayCard_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray< FCardStruct>& Sacrifice);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Server")
	static bool CanAttack_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& AttackerStruct, const FCardStruct& DefenderStruct);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Server")
	static bool CanActivate_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice);
	//***************************************************************
	
	//***********************服务器原子事件**************************
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanMoveCard(ACardCoreDriver* Driver, UCardInstance* Card, EZone FromZone, EZone ToZone, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanAttachCard(ACardCoreDriver* Driver, UCardInstance* SourceCard, UCardInstance* TargetCard, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanDeclareCardAttack(ACardCoreDriver* Driver, UCardInstance* Attacker, UCardInstance* Defender, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanApplyDamage(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Damage, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanApplyHeal(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Heal, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanActivateCardWithSacrifices(ACardCoreDriver* Driver, UCardInstance* Card, const TArray<UCardInstance*>& Sacrifices, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanUpdateCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanApplyEffectToInstance(ACardCoreDriver* Driver, UCardInstance* Source, UEffectInstance* TargetEffect, EReason Reason);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Atomic")
	static bool CanRevealCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason);
	//***************************************************************
	
	//*************************游戏流程******************************
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Flow")
	static bool CanNormalDraw(ACardCoreDriver* Driver, int PlayerIndex, int NumCards);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Flow")
	static bool CanNormalDrawToHandSize(ACardCoreDriver* Driver, int PlayerIndex, int TargetHandSize);
	//***************************************************************
	
	//************************解释性事件*****************************
	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanSummonServant(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanCastSpell(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanBuildTerrain(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanAttachEquip(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanPaySacrificeCost(ACardCoreDriver* Driver, int SacrificeCardIndex, int RelativeCardIndex);

	UFUNCTION(BlueprintCallable, Category = "RuleChecker|Resolution")
	static bool CanTryMoveSpellToEchoOrGrave(ACardCoreDriver* Driver, int SourceCardIndex, int RelativeCardIndex);
	//***************************************************************
};
