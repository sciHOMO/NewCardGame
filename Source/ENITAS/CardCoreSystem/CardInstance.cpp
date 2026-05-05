#include "../CardCoreSystem/CardInstance.h"

bool UCardInstance::ClientValidateHaveSacrifices_Implementation(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& AllSacrificeCards)
{
	return true;
}

bool UCardInstance::ClientValidatePlaySacrifices_Implementation(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& PickUpSacrificeCards)
{
	return true;
}

bool UCardInstance::ClientValidateFoundSacrifice_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SacrificeStruct)
{
	return true;
}
