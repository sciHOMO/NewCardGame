#include "../CardCoreSystem/CardInstance.h"

bool UCardInstance::ClientValidateHaveSacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& SacStructArray)
{
	if (SacStructArray.Num() < SourceCardStruct.CardLevel) return false;
	return true;
}

bool UCardInstance::ClientValidatePlaySacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& SacStructArray)
{
	if (SacStructArray.Num() != SourceCardStruct.CardLevel) return false;
	return true;
}

bool UCardInstance::ClientValidateFoundSacrifice_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const FCardStruct& SacStruct)
{
	(void)OuterDriver;
	(void)SourceCardStruct;
	(void)SacStruct;
	return true;
}

bool UCardInstance::ServerValidatePlaySacrifices_Implementation(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& SacStructArray)
{
	return ClientValidatePlaySacrifices(OuterDriver, SourceCardStruct, SacStructArray);
}
