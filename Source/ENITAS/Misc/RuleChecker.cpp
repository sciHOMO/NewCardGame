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
		
		return CardCDO -> ClientValidateHaveSacrifices(Driver, Sacrifice);
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
	(void)Sacrifice;
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

		return EffectCDO -> ClientValidateHaveSacrifices(Driver, Sacrifice);
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
		
		return CardCDO -> ClientValidateFoundSacrifice(Driver, SacrificeStruct);
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
		
		return EffectCDO -> ClientValidateFoundSacrifice(Driver, SacrificeStruct);
	}
}

bool URuleChecker::IsNecessarySacrificesForPlay(const ACardCoreDriver* Driver, const int PlayerIndex, const FCardStruct& CardStruct, const TArray<FCardStruct>& Sacrifice)
{
	{
		if (!CardStruct.CardInstanceClass) return false;

		UCardInstance* CardCDO = CardStruct.CardInstanceClass -> GetDefaultObject<UCardInstance>();
		if (!CardCDO) return false;
		
		return CardCDO -> ClientValidatePlaySacrifices(Driver, Sacrifice);
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
		
		return EffectCDO -> ClientValidateActivateSacrifices(Driver, Sacrifice);
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
		
		return CardCDO -> ClientValidatePlaySacrifices(Driver, Sacrifice);
	}
}


//*************************************************************************

bool URuleChecker::CanMoveCard(ACardCoreDriver* Driver, UCardInstance* Card, EZone FromZone, EZone ToZone, EReason Reason,
	FText& OutFailureReason)
{
	(void)Driver;
	(void)Card;
	(void)FromZone;
	(void)ToZone;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanAttachCard(ACardCoreDriver* Driver, UCardInstance* SourceCard, UCardInstance* TargetCard,
	EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCard;
	(void)TargetCard;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanDeclareCardAttack(ACardCoreDriver* Driver, UCardInstance* Attacker, UCardInstance* Defender,
	EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Attacker;
	(void)Defender;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanApplyDamage(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Damage,
	EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Source;
	(void)Target;
	(void)Damage;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanApplyHeal(ACardCoreDriver* Driver, UCardInstance* Source, UCardInstance* Target, int Heal,
	EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Source;
	(void)Target;
	(void)Heal;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanActivateCardWithSacrifices(ACardCoreDriver* Driver, UCardInstance* Card,
	const TArray<UCardInstance*>& Sacrifices, EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Card;
	(void)Sacrifices;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanUpdateCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Card;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanApplyEffectToInstance(ACardCoreDriver* Driver, UCardInstance* Source, UEffectInstance* TargetEffect,
	EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Source;
	(void)TargetEffect;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanRevealCard(ACardCoreDriver* Driver, UCardInstance* Card, EReason Reason, FText& OutFailureReason)
{
	(void)Driver;
	(void)Card;
	(void)Reason;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanNormalDraw(ACardCoreDriver* Driver, int PlayerIndex, int NumCards, FText& OutFailureReason)
{
	(void)Driver;
	(void)PlayerIndex;
	(void)NumCards;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanNormalDrawToHandSize(ACardCoreDriver* Driver, int PlayerIndex, int TargetHandSize,
	FText& OutFailureReason)
{
	(void)Driver;
	(void)PlayerIndex;
	(void)TargetHandSize;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanSummonServant(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex,
	const TArray<int>& SacrificeIndices, FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCardIndex;
	(void)TargetCardIndex;
	(void)SacrificeIndices;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanCastSpell(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex,
	const TArray<int>& SacrificeIndices, FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCardIndex;
	(void)TargetCardIndex;
	(void)SacrificeIndices;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanBuildTerrain(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex,
	const TArray<int>& SacrificeIndices, FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCardIndex;
	(void)TargetCardIndex;
	(void)SacrificeIndices;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanAttachEquip(ACardCoreDriver* Driver, int SourceCardIndex, int TargetCardIndex,
	const TArray<int>& SacrificeIndices, FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCardIndex;
	(void)TargetCardIndex;
	(void)SacrificeIndices;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanPaySacrificeCost(ACardCoreDriver* Driver, int SacrificeCardIndex, int RelativeSpellOrPlayCardIndex,
	FText& OutFailureReason)
{
	(void)Driver;
	(void)SacrificeCardIndex;
	(void)RelativeSpellOrPlayCardIndex;
	(void)OutFailureReason;
	return false;
}

bool URuleChecker::CanTryMoveSpellToEchoOrGrave(ACardCoreDriver* Driver, int SourceCardIndex, int RelativeCardIndex,
	FText& OutFailureReason)
{
	(void)Driver;
	(void)SourceCardIndex;
	(void)RelativeCardIndex;
	(void)OutFailureReason;
	return false;
}

