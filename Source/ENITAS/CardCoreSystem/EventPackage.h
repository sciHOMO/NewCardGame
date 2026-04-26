#pragma once

#include "CoreMinimal.h"
#include "../CardCoreSystem/CardInstance.h"
#include "EventPackage.generated.h"

/*
事件包结构体
*/

//事件技术方案简介：
//自解释结构体，Param通过枚举型标识异构参数
//自解释结构体，Package通过枚举型挂载异构参数列表
//基于统一发送器EventDispatach转发

UENUM(BlueprintType)
enum class EParamType : uint8
{
	PlaceHolder				UMETA(DisplayName = "PlayerHolder"),
	CardOrPlayer				UMETA(DisplayName = "CardOrPlayer"),
	ZoneValue					UMETA(DisplayName = "ZoneValue"),
	IntValue						UMETA(DisplayName = "IntValue"),
	IndexArray					UMETA(DisplayName = "IndexArray"),
	XReason						UMETA(DisplayName = "XReason"),
	Description					UMETA(DisplayName = "Description"),
};

UENUM(BlueprintType)
enum class EReason : uint8
{
	PlaceHolder			UMETA(DisplayName = "PlaceHolder"),
};

USTRUCT(Blueprintable, BlueprintType)
struct  FEventParamStruct
{
	GENERATED_BODY()
	
	//****************************参数核心********************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EParamType ParamType = EParamType::PlaceHolder;	//参数类型

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FCardStruct CardOrPlayer;	//卡牌结构体参数

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EZone ZoneValue = EZone::PlaceHolder;	//位置参数

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int IntValue = INT_ERROR;		//整型参数

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<int> IndexArray = {};	//多张卡牌传递序号数组参数

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EReason Reason = EReason::PlaceHolder;	
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Description;	//描述性文字
	//********************************************************************
	
	//****************************构造方法*******************************
	static FEventParamStruct MakeCardOrPlayer(const FCardStruct& Value)	//基于卡牌结构体构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::CardOrPlayer;
		Param.CardOrPlayer = Value;
		return Param;
	}

	static FEventParamStruct MakeZone(const EZone Value)	//基于位置构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::ZoneValue;
		Param.ZoneValue = Value;
		return Param;
	}

	static FEventParamStruct MakeInt(const int Value)	//基于整型构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::IntValue;
		Param.IntValue = Value;
		return Param;
	}

	static FEventParamStruct MakeIndexArray(const TArray<int>& Value)	//基于序号数组构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::IndexArray;
		Param.IndexArray = Value;
		return Param;
	}

	static FEventParamStruct MakeReason(const EReason Reason)	//基于序号数组构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::XReason;
		Param.Reason=Reason;
		return Param;
	}
	
	static FEventParamStruct MakeDescription(const FText& Value)	//基于序号数组构造
	{
		FEventParamStruct Param;
		Param.ParamType = EParamType::Description;
		Param.Description = Value;
		return Param;
	}
	
	//********************************************************************
};

UENUM(BlueprintType)
enum class EPackageType : uint8
{
	PlaceHolder				UMETA(DisplayName = "PlayerHolder"),
	GameStart					UMETA(DisplayName = "GameStart"),
	GameEnd					UMETA(DisplayName = "GameEnd"),					//[0] 胜利玩家ID：IntValue
	TurnStart						UMETA(DisplayName = "TurnStart"),						//[0] 回合玩家ID：IntValue
	TurnEnd						UMETA(DisplayName = "TurnEnd"),						//[0] 回合玩家ID：IntValue
	CardMove					UMETA(DisplayName = "CardMove"),					//[0] 卡牌结构体：CardOrPlayer [1] 位置：FromZoneValue
	CardAttach					UMETA(DisplayName = "CardAttach"),					//[0] 卡牌结构体：CardOrPlayer [1] 卡牌结构体：CardOrPlayer 
	CardAttack					UMETA(DisplayName = "CardAttack"),					//[0] 卡牌结构体：CardOrPlayer [1] 卡牌结构体：CardOrPlayer 
	CardApplyDamage		UMETA(DisplayName = "CardApplyDamage"),		//[0] 卡牌结构体：CardOrPlayer [1] 伤害数值：IntValue
	CardApplyHeal			UMETA(DisplayName = "CardApplyHeal"),			//[0] 卡牌结构体：CardOrPlayer [1] 恢复数值：IntValue
	CardActivate				UMETA(DisplayName = "CardActivate"),				//[0] 卡牌结构体：CardOrPlayer
	CardUpdate					UMETA(DisplayName = "CardUpdate"),				//[0] 卡牌结构体：CardOrPlayer
	CardApplyEffect			UMETA(DisplayName = "CardApplyEffect"),			//[0] 卡牌结构体：CardOrPlayer
	CardReveal					UMETA(DisplayName = "CardReveal"),					//[0] 卡牌结构体：CardOrPlayer
	TargetPickUp				UMETA(DisplayName = "TargetPickUp"),				//[0] 对象数组：IndexArray
};

USTRUCT(Blueprintable, BlueprintType)
struct  FEventPackageStruct
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EPackageType PackageType = EPackageType::PlaceHolder;	//事件类型

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<FEventParamStruct> Params;	//参数列表

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<int> FocusPlayerIndex = {};	//关注并处理的客户端
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int64 GlobalEventIndex = INT_ERROR;	//时间序列戳

	bool IsValid() const
	{
		return GlobalEventIndex != INT_ERROR;
	}

	bool operator==(const FEventPackageStruct& Other) const
	{
		return GlobalEventIndex == Other.GlobalEventIndex;
	}
};

	