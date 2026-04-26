#pragma once

#include "CoreMinimal.h"
#include "EffectContext.generated.h"

class UCardInstance;

/*
卡牌效果上下文基于时机定制。
例如，当其他生物被召唤时，需知道其他生物，则上下文包含CardInstance。
*/

//时机枚举型
UENUM(BlueprintType)
enum class ECondition : uint8
{
	PlaceHolder					UMETA(DisplayName = "PlaceHolder"),
	Activate							UMETA(DisplayName = "Activate"),
	BattleCry							UMETA(DisplayName = "BattleCry"),
	DeathRattle						UMETA(DisplayName = "DeathRattle"),
	CastSpell							UMETA(DisplayName = "CastSpell"),
	OnSummonServant			UMETA(DisplayName = "OnSummonServant"),
	OnCastSpell					UMETA(DisplayName = "OnCastSpell"),
	OnBuildTerrain				UMETA(DisplayName = "OnBuildTerrain"),
	OnAttachEquip				UMETA(DisplayName = "OnAttachEquip"),
	OnSacrificed					UMETA(DisplayName = "OnSacrificed"),
	OnActivate						UMETA(DisplayName = "OnActivate"),
	OnBattleCry						UMETA(DisplayName = "OnBattleCry"),
	OnDeathRattle				UMETA(DisplayName = "OnDeathRattle"),

};

UCLASS()
class UEffectContext : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECondition Condition = ECondition::PlaceHolder;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* PrePickUp;
};

UCLASS()
class UEffectContextForActivate : public UEffectContext
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> Sacrifice;
};

UCLASS()
class UEffectContextForBattleCry : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> Sacrifice;
};

UCLASS()
class UEffectContextForDeathRattle : public UEffectContext
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* RelativeCard;
};

UCLASS()
class UEffectContextForCastSpell : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> Sacrifice;
};

UCLASS()
class UEffectContextForOnSummonServant : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnCastSpell : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnBuildTerrain : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnAttachEquip : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnSacrificed : public UEffectContext
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* RelativeCard;
};

UCLASS()
class UEffectContextForOnActivate : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnBattleCry : public UEffectContext
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
};

UCLASS()
class UEffectContextForOnDeathRattle : public UEffectContext
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* SourceCard;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* RelativeCard;
};

UCLASS()
class UEffectContextForCondition_10 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_11 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_12 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_13 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_14 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_15 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_16 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_17 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_18 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_19 : public UEffectContext
{
	GENERATED_BODY()
};

UCLASS()
class UEffectContextForCondition_20 : public UEffectContext
{
	GENERATED_BODY()
};