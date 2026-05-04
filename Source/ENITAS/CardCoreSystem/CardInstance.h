#pragma once

#include "CoreMinimal.h"
#include "CardInstance.generated.h"

enum class ECondition : uint8;
struct FEffectContext;
class UCardInstance;
class UEffectInstance;
class ACardCoreDriver;
class ACardModel;

/*
 卡牌结构体与包裹体实例
 通过绑定实例类间接实现多态
*/

//卡牌技术方案简介：
//采用FCardStruct结构体承载核心参数 + UCardInstance包裹结构体的形式作为卡牌的基础。
//FCardStruct可承载数据，复制友好，但未实现多态，难以挂载卡牌效果
//UCardInstance可实现多态挂载卡牌效果，但服务器权威存在无法复制
//解决方案为，FCardStruct挂载UCardInstance类，在服务器上生成UCardInstance然后包裹该FCardStruct
//服务器-客户端通信主要依赖FCardStruct，理论上客户端不关心具体效果；效果仅仅是文本
//具体的效果在UCardInstance内通过TMap定义

//卡牌位置
UENUM(Blueprintable, BlueprintType)
enum class EZone : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
	DeckZone				UMETA(DisplayName = "DeckZone"),
	HandZone				UMETA(DisplayName = "HandZone"),
	BoardZone				UMETA(DisplayName = "BoardZone"),
	EchoZone				UMETA(DisplayName = "EchoZone"),
	GraveZone				UMETA(DisplayName = "GraveZone"),
};

//卡牌类型
UENUM(Blueprintable, BlueprintType)
enum class EType : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
	Servant					UMETA(DisplayName = "Servant"),
	Spell						UMETA(DisplayName = "Spell"),
	Terrain						UMETA(DisplayName = "Terrain"),
	Equip						UMETA(DisplayName = "Equip"),
};

//卡牌色轮
UENUM(Blueprintable, BlueprintType)
enum class EColor : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
	Black						UMETA(DisplayName = "Black"),
	White						UMETA(DisplayName = "White"),
	Azure						UMETA(DisplayName = "Azure"),
	Red							UMETA(DisplayName = "Red"),
	Green						UMETA(DisplayName = "Green"),
};

//卡牌罕贵
UENUM(Blueprintable, BlueprintType)
enum class ERarity : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
	N								UMETA(DisplayName = "N"),
	R								UMETA(DisplayName = "R"),
	SR							UMETA(DisplayName = "SR"),
	SSR							UMETA(DisplayName = "SSR"),
};

//卡牌职业
UENUM(Blueprintable, BlueprintType)
enum class EClass : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
};

//卡牌种族
UENUM(Blueprintable, BlueprintType)
enum class ERace : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
};

USTRUCT(Blueprintable, BlueprintType)
struct FCardStruct : public FTableRowBase
{
	GENERATED_BODY()
	
	//****************************局内生成********************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int CardIndex = INT_ERROR;	//卡牌唯一序列号

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int PlayerIndex = INT_ERROR;	//拥有者序列号

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	bool Tapped = false;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int Cur_AP = 0;	//卡牌数值

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int Cur_HP = 0;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int Max_HP = HP;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EZone CardZone = EZone::DeckZone;	//卡牌位置
	//********************************************************************
	
	//****************************局外定义********************************
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CardID = INT_ERROR;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName CardName = TEXT("Name");

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int CardLevel = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EType CardType = EType::PlaceHolder;	//卡牌类型

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EColor CardColor = EColor::PlaceHolder;	//卡牌色轮

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERarity CardRarity = ERarity::PlaceHolder;	//卡牌罕贵

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EClass CardClass = EClass::PlaceHolder;	//卡牌色轮

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ERace CardRace = ERace::PlaceHolder;	//卡牌罕贵

	UPROPERTY(BlueprintReadWrite, EditAnywhere)	
	int AP = 0;	//卡牌数值

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int HP = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UCardInstance> CardInstanceClass;	//实例类

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<ACardModel> CardModelClass;
	//********************************************************************
};

UCLASS(Blueprintable, BlueprintType)
class UCardInstance : public UObject
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	ACardCoreDriver* Driver = nullptr;		//驱动器

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FCardStruct CardStruct = FCardStruct(); //结构体引用 包裹

	//复数个效果，需要查找
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<ECondition, TSubclassOf<UEffectInstance>> EffectForCondition;	//效果定义

	UFUNCTION(BlueprintNativeEvent)
	bool ClientValidateHaveSacrifices(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& AllSacrificeCards);	//检测是否有合法代价

	UFUNCTION(BlueprintNativeEvent)
	bool ClientValidatePickUpSacrifices(const ACardCoreDriver* OuterDriver, const TArray<FCardStruct>& PickUpSacrificeCards);	//检测当前已选择的代价
};
