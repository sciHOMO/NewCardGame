#pragma once

#include "CoreMinimal.h"
#include "../CardCoreSystem/CardInstance.h"
#include "EffectInstance.generated.h"

class UCardInstance;
class UEffectContext;
class ACardCoreDriver;

/*
效果包裹体实例
 */

//效果技术方案简介：
//采用UEffectInstance作为效果的基础。
//服务器模拟事件总线询问全部卡牌是否有效果需要触发
//通过CDO方法检测UEffectInstance是否应当响应那个触发时机，是否有对象（时机/目标校验，代价参见 ClientValidateActivateSacrifices；当局源牌用 SourceCardStruct 传入）
//创建效果UEffectInstance后压入Stack中逐个执行
//执行流程由UEffectInstance的Activate、Complete函数控制

UCLASS(Blueprintable, BlueprintType)
class UEffectInstance : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UCardInstance* CardInstance = nullptr;	//来源卡牌实例
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UEffectContext* StoredContext = nullptr;	//效果上下文
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	ACardCoreDriver* Driver = nullptr;	//全局信息

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int EffectLevel = 0;	//效果等级

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString EffectSacrifice = TEXT("选择一个对象");	//代价说明
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int TargetNum = 0;	//目标数量
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UCardInstance*> TargetPicked = {};	//目标缓存池
	
	UFUNCTION(BlueprintNativeEvent)
	bool CheckCondition_Effect(UEffectContext* OuterContext);	//基于CDO检测时机

	UFUNCTION(BlueprintNativeEvent)
	bool CheckTargets_Effect(ACardCoreDriver* OuterDriver);	//基于CDO检测对象

	UFUNCTION(BlueprintNativeEvent)
	TArray<int> GetValidTargets();	//目标筛选器	

	/** SourceCardStruct：当局发动效果的那张场上牌。 */
	UFUNCTION(BlueprintNativeEvent)
	bool ClientValidateHaveSacrifices(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& AllSacrificeCards);
	
	UFUNCTION(BlueprintNativeEvent)
	bool ClientValidateActivateSacrifices(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const TArray<FCardStruct>& PickUpSacrificeCards);

	UFUNCTION(BlueprintNativeEvent)
	bool ClientValidateFoundSacrifice(const ACardCoreDriver* OuterDriver, const FCardStruct& SourceCardStruct, const FCardStruct& SacrificeStruct);
	
	UFUNCTION()
	void Activate();	//通过Stack点火开始结算

	UFUNCTION()
	void Require();	//请求对象

	UFUNCTION()
	void CallBack(UCardInstance* PickedTarget);	//完成对象列表
	
	UFUNCTION()
	virtual void Execute();	//执行体

	UFUNCTION()
	void Notify() const;	//诱发

	UFUNCTION()
	void Complete() const;	//再点火;
};
