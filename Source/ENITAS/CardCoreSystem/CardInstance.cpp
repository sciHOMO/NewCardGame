#include "../CardCoreSystem/CardInstance.h"

bool UCardInstance::ClientValidateHaveSacrifices_Implementation(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& AllSacrificeCards)
{
	return true;
}

bool UCardInstance::ClientValidatePickUpSacrifices_Implementation(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& PickUpSacrificeCards)
{
	return true;
}
