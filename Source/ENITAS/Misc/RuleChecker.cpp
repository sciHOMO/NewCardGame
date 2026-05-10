#include "../Misc/RuleChecker.h"
#include "../CardCoreSystem/CardCoreDriver.h"
#include "../CardCoreSystem/EffectContext.h"
#include "../CardCoreSystem/EffectInstance.h"

//****************************玩家输入预检测******************************
bool URuleChecker::CanEndTurn_Client(const ACardCoreDriver* Driver, const int PlayerIndex)
{
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	
	return true;
}

bool URuleChecker::CanPlayCard_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray< FCardStruct>& Sacrifice)
{
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::HandZone && CardStruct.CardZone != EZone::PlaceHolder) return false;

	{
		if (!CardStruct.CardInstanceClass) return false;

		UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;
		
		return CardCDO -> ClientValidateHaveSacrifices(Driver, CardStruct, Sacrifice);
	}
}

bool URuleChecker::CanAttackOrActivate_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct)
{
	if (!Driver) return false;
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::BoardZone) return false;

	return true;
}

bool URuleChecker::CanAttack_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& AttackerStruct, const FCardStruct& DefenderStruct)
{
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != AttackerStruct.PlayerIndex) return false;
	if (PlayerIndex == DefenderStruct.PlayerIndex) return false;
	if (AttackerStruct.CardZone != EZone::BoardZone) return false;
	if (DefenderStruct.CardZone != EZone::BoardZone) return false;
	if (AttackerStruct.Tapped) return false;

	return true;
}


bool URuleChecker::CanActivate_Client(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::BoardZone) return false;
	if (CardStruct.Tapped) return false;
	
	{
		if (!CardStruct.CardInstanceClass) return false;

		const UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;

		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardCDO->EffectForCondition.Find(ECondition::Activate);
		if (!EffectClassPtr || !(*EffectClassPtr)) return false;

		UEffectInstance* EffectCDO = (*EffectClassPtr) -> GetDefaultObject<UEffectInstance>();
		if (!EffectCDO) return false;

		return EffectCDO -> ClientValidateHaveSacrifices(Driver, CardStruct, Sacrifice);
	}
}

bool URuleChecker::IsValidSacrificeForPlay(const ACardCoreDriver* Driver, const int PlayerIndex,const FCardStruct& CardStruct, const FCardStruct& SacrificeStruct)
{
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (PlayerIndex != SacrificeStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::HandZone && CardStruct.CardZone != EZone::PlaceHolder) return false;
	if (SacrificeStruct.CardZone != EZone::HandZone && SacrificeStruct.CardZone != EZone::EchoZone) return false;
	if (SacrificeStruct.CardIndex == CardStruct.CardIndex) return false;

	{
		if (!CardStruct.CardInstanceClass) return false;

		UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;
		
		return CardCDO -> ClientValidateFoundSacrifice(Driver, CardStruct, SacrificeStruct);
	}
}

bool URuleChecker::IsValidSacrificeForEffect(const ACardCoreDriver* Driver, const int PlayerIndex,const FCardStruct& CardStruct, const FCardStruct& SacrificeStruct)
{
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (PlayerIndex != SacrificeStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::BoardZone) return false;
	if (SacrificeStruct.CardZone != EZone::HandZone && SacrificeStruct.CardZone != EZone::EchoZone) return false;
	if (SacrificeStruct.CardIndex == CardStruct.CardIndex) return false;

	{
		if (!CardStruct.CardInstanceClass) return false;
		
		const UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;

		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardCDO->EffectForCondition.Find(ECondition::Activate);
		if (!EffectClassPtr || !(*EffectClassPtr)) return false;

		UEffectInstance* EffectCDO = (*EffectClassPtr) -> GetDefaultObject<UEffectInstance>();
		if (!EffectCDO) return false;
		
		return EffectCDO -> ClientValidateFoundSacrifice(Driver, CardStruct, SacrificeStruct);
	}
}

bool URuleChecker::IsNecessarySacrificesForPlay(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	{
		if (!CardStruct.CardInstanceClass) return false;

		UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;
		
		return CardCDO -> ClientValidatePlaySacrifices(Driver, CardStruct, Sacrifice);
	}
}

bool URuleChecker::IsNecessarySacrificesForEffect(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	{
		if (!CardStruct.CardInstanceClass) return false;
		
		const UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;

		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardCDO->EffectForCondition.Find(ECondition::Activate);
		if (!EffectClassPtr || !(*EffectClassPtr)) return false;

		UEffectInstance* EffectCDO = (*EffectClassPtr) -> GetDefaultObject<UEffectInstance>();
		if (!EffectCDO) return false;
		
		return EffectCDO -> ClientValidateActivateSacrifices(Driver, CardStruct, Sacrifice);
	}
}
//*************************************************************************

//****************************服务器校验**********************************
bool URuleChecker::CanEndTurn_Server(const ACardCoreDriver* Driver, const int PlayerIndex)
{
	if (!Driver) return false;
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;

	return true;
}

bool URuleChecker::CanPlayCard_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	if (!Driver) return false;
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::HandZone && CardStruct.CardZone != EZone::PlaceHolder) return false;

	for (const FCardStruct& Idx : Sacrifice)
	{
		if (Idx.PlayerIndex != PlayerIndex)
		{
			return false;
		}
		if (Idx.CardZone != EZone::HandZone && Idx.CardZone != EZone::EchoZone)
		{
			return false;
		}
		if (Idx.CardIndex == CardStruct.CardIndex)
		{
			return false;
		}
	}
	
	{
		if (!CardStruct.CardInstanceClass) return false;

		UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;
		
		return CardCDO -> ClientValidatePlaySacrifices(Driver, CardStruct, Sacrifice);
	}
}

bool URuleChecker::CanAttack_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& AttackerStruct, const FCardStruct& DefenderStruct)
{
	if (!Driver) return false;
	if (Driver -> TurnNum == 1) return false;
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != AttackerStruct.PlayerIndex) return false;
	if (PlayerIndex == DefenderStruct.PlayerIndex) return false;
	if (AttackerStruct.CardZone != EZone::BoardZone) return false;
	if (DefenderStruct.CardZone != EZone::BoardZone) return false;
	if (AttackerStruct.Tapped) return false;

	return true;
}

bool URuleChecker::CanActivate_Server(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	if (!Driver) return false;
	if (Driver -> GamePhase == EPhase::Player_0_Turn && PlayerIndex) return false;
	if (Driver -> GamePhase == EPhase::Player_1_Turn && !PlayerIndex) return false;
	if (PlayerIndex != CardStruct.PlayerIndex) return false;
	if (CardStruct.CardZone != EZone::BoardZone) return false;
	if (CardStruct.Tapped) return false;

	for (const FCardStruct& Idx : Sacrifice)
	{
		if (Idx.PlayerIndex != PlayerIndex)
		{
			return false;
		}
		if (Idx.CardZone != EZone::HandZone && Idx.CardZone != EZone::EchoZone)
		{
			return false;
		}
		if (Idx.CardIndex == CardStruct.CardIndex)
		{
			return false;
		}
	}

	{
		if (!CardStruct.CardInstanceClass) return false;

		const UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;

		const TSubclassOf<UEffectInstance>* EffectClassPtr = CardCDO->EffectForCondition.Find(ECondition::Activate);
		if (!EffectClassPtr || !(*EffectClassPtr)) return false;

		UEffectInstance* EffectCDO = (*EffectClassPtr) -> GetDefaultObject<UEffectInstance>();
		if (!EffectCDO) return false;

		return EffectCDO -> ClientValidateActivateSacrifices(Driver, CardStruct, Sacrifice);
	}
}
//*************************************************************************

//****************************服务器原子事件**********************************
bool URuleChecker::CanMoveCard(ACardCoreDriver* Driver, UCardInstance* Card, EZone FromZone, EZone ToZone, EReason Reason)
{
	if (!Driver || !Card) return false;
	if (FromZone == EZone::PlaceHolder || ToZone == EZone::PlaceHolder) return false;
	if (Card -> CardStruct.CardZone != FromZone) return false;
	if (FromZone == ToZone) return false;

	if (Reason == EReason::Sacrifice)
	{
		if (FromZone != EZone::HandZone && FromZone != EZone::EchoZone) return false;
		if (ToZone != EZone::GraveZone) return false;
	}

	return true;
}

bool URuleChecker::CanAttachCard(ACardCoreDriver* Driver, UCardInstance* SourceCard, UCardInstance* TargetCard, EReason Reason)
{
	if (!Driver || !SourceCard || !TargetCard) return false;
	if (SourceCard -> CardStruct.PlayerIndex != TargetCard -> CardStruct.PlayerIndex) return false;
	if (TargetCard -> CardStruct.CardZone != EZone::BoardZone) return false;
	if (SourceCard -> CardStruct.CardType != EType::Equip) return false;
	if (SourceCard -> CardStruct.CardZone != EZone::BoardZone) return false;

	return true;
}

bool URuleChecker::CanDeclareCardAttack(ACardCoreDriver* Driver, UCardInstance* Attacker, UCardInstance* Defender, EReason Reason)
{
	if (!Driver || !Attacker || !Defender) return false;
	return CanAttack_Server(Driver, Attacker -> CardStruct.PlayerIndex, Attacker -> CardStruct, Defender -> CardStruct);
}

bool URuleChecker::CanApplyDamage(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Damage, EReason Reason)
{
	if (!Driver || !Source || !Target) return false;
	if (Damage < 0) return false;
	
	if (Target -> CardStruct.CardZone != EZone::BoardZone) return false;
	if (Target -> CardStruct.Cur_HP <= 0) return false;

	return true;
}

bool URuleChecker::CanApplyHeal(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Heal, EReason Reason)
{
	if (!Driver || !Source || !Target) return false;
	if (Heal < 0) return false;
	
	if (Target -> CardStruct.CardZone != EZone::BoardZone) return false;
	if (Target -> CardStruct.Cur_HP <= 0) return false;
	
	return true;
}

bool URuleChecker::CanActivateCardWithSacrifices(ACardCoreDriver* Driver, UCardInstance* Card, const TArray<UCardInstance*>& Sacrifices, EReason Reason)
{
	if (!Driver || !Card) return false;
	
	TArray<FCardStruct> SacrificeStructs;
	for (UCardInstance* Idx : Sacrifices)
	{
		if (!Idx) return false;
		SacrificeStructs.Emplace(Idx -> CardStruct);
	}
	
	return CanActivate_Server(Driver, Card -> CardStruct.PlayerIndex, Card -> CardStruct, SacrificeStructs);
}

bool URuleChecker::CanUpdateCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason)
{
	return Driver && Card;
}

bool URuleChecker::CanApplyEffectToInstance(ACardCoreDriver* Driver, UCardInstance* Source, UEffectInstance* TargetEffect, EReason Reason)
{
	return Driver && Source && TargetEffect;
}

bool URuleChecker::CanRevealCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason)
{
	return Driver && Card;
}

bool URuleChecker::CanNormalDraw(ACardCoreDriver* Driver, int PlayerIndex, int NumCards)
{
	if (!Driver) return false;
	if (NumCards <= 0) return false;
	
	return true;
}

bool URuleChecker::CanNormalDrawToHandSize(ACardCoreDriver* Driver, int PlayerIndex, int TargetHandSize)
{
	if (!Driver) return false;
	if (TargetHandSize <= 0) return false;
	
	return true;
}

bool URuleChecker::CanSummonServant(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices)
{
	if (!Driver) return false;

	const UCardInstance* Source = Driver -> GetCardInstanceByIndex(SourceCardIndex);
	if (!Source) return false;
	if (Source -> CardStruct.CardType != EType::Servant) return false;
	if (Source -> CardStruct.CardZone != EZone::HandZone) return false;
	if (Source -> CardStruct.PlayerIndex == INT_ERROR) return false;

	{
		const int BoardN = Driver -> GetZoneSizeByPlayerIndexAndZone(Source -> CardStruct.PlayerIndex, EZone::BoardZone);
		if (BoardN == INT_ERROR || BoardN >= 7) return false;
	}
	
	return true;
}

bool URuleChecker::CanCastSpell(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices)
{
	if (!Driver) return false;

	UCardInstance* const Source = Driver -> GetCardInstanceByIndex(SourceCardIndex);
	if (!Source) return false;
	if (Source -> CardStruct.CardType != EType::Spell) return false;
	if (Source -> CardStruct.CardZone != EZone::HandZone) return false;
	if (Source -> CardStruct.PlayerIndex == INT_ERROR) return false;
	
	return true;
}

bool URuleChecker::CanBuildTerrain(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices)
{
	if (!Driver) return false;

	const UCardInstance* Source = Driver -> GetCardInstanceByIndex(SourceCardIndex);
	if (!Source) return false;
	if (Source -> CardStruct.CardType != EType::Terrain) return false;
	if (Source -> CardStruct.CardZone != EZone::HandZone) return false;
	if (Source -> CardStruct.PlayerIndex == INT_ERROR) return false;

	{
		const int BoardN = Driver -> GetZoneSizeByPlayerIndexAndZone(Source -> CardStruct.PlayerIndex, EZone::BoardZone);
		if (BoardN == INT_ERROR || BoardN >= 7) return false;
	}
	
	return true;
}

bool URuleChecker::CanAttachEquip(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex, const TArray<int>& SacrificeIndices)
{
	if (!Driver) return false;

	const UCardInstance* Source = Driver -> GetCardInstanceByIndex(SourceCardIndex);
	const UCardInstance* Target = Driver -> GetCardInstanceByIndex(TargetCardIndex);
	if (!Source || !Target) return false;
	if (Source -> CardStruct.CardType != EType::Equip) return false;
	if (Source -> CardStruct.CardZone != EZone::HandZone) return false;
	if (Source -> CardStruct.PlayerIndex == INT_ERROR || Target -> CardStruct.PlayerIndex == INT_ERROR) return false;
	if (Source -> CardStruct.PlayerIndex != Target -> CardStruct.PlayerIndex) return false;
	
	if (Target -> CardStruct.CardZone != EZone::BoardZone) return false;

	return true;
}

bool URuleChecker::CanPaySacrificeCost(ACardCoreDriver* Driver, int SacrificeCardIndex, int RelativeCardIndex)
{
	if (!Driver) return false;

	const UCardInstance* Sacrifice = Driver -> GetCardInstanceByIndex(SacrificeCardIndex);
	const UCardInstance* Relative = Driver -> GetCardInstanceByIndex(RelativeCardIndex);
	if (!Sacrifice || !Relative) return false;

	if (Sacrifice -> CardStruct.PlayerIndex != Relative -> CardStruct.PlayerIndex) return false;
	if (Sacrifice -> CardStruct.CardZone != EZone::HandZone && Sacrifice -> CardStruct.CardZone != EZone::EchoZone) return false;
	if (Sacrifice -> CardStruct.CardIndex == Relative -> CardStruct.CardIndex) return false;

	return true;
}

bool URuleChecker::CanTryMoveSpellToEchoOrGrave(ACardCoreDriver* Driver, int SourceCardIndex, int RelativeCardIndex)
{
	if (!Driver) return false;

	const UCardInstance* Source = Driver -> GetCardInstanceByIndex(SourceCardIndex);
	if (!Source) return false;
	if (Source -> CardStruct.CardType != EType::Spell) return false;
	if (Source -> CardStruct.CardZone != EZone::HandZone) return false;

	if (RelativeCardIndex == INT_ERROR || !Driver -> GetCardInstanceByIndex(RelativeCardIndex)) return false;
	
	return true;
}

