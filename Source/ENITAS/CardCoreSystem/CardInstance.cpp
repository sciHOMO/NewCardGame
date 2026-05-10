#include "../CardCoreSystem/CardInstance.h"

bool UCardInstance::ClientValidateHaveSacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& AllSacrificeCards)
{
	if (AllSacrificeCards.Num() < SourceCardStruct.CardLevel) return false;
	return true;
}

bool UCardInstance::ClientValidatePlaySacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& PickUpSacrificeCards)
{
	if (PickUpSacrificeCards.Num() != SourceCardStruct.CardLevel) return false;
	return true;
}

bool UCardInstance::ClientValidateFoundSacrifice_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const FCardStruct& SacrificeStruct)
{
	(void)OuterDriver;
	(void)SourceCardStruct;
	(void)SacrificeStruct;
	return true;
}

bool UCardInstance::ServerValidatePlaySacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& PickUpSacrificeCards)
{
	return ClientValidatePlaySacrifices(OuterDriver, SourceCardStruct, PickUpSacrificeCards);
}
