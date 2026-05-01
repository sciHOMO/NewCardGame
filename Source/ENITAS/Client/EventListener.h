#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../CardCoreSystem/EventPackage.h"
#include "GameFramework/PlayerState.h"
#include "EventListener.generated.h"

class ACardModel;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UEventListener : public UActorComponent
{
	GENERATED_BODY()

	UEventListener();

public:
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	AController* Controller = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int LocalEventIndex = 0;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<FEventPackageStruct> PendingQueue = {};

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<FEventPackageStruct> ProcessedQueue = {};

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FEventPackageStruct ProcessingPackage = FEventPackageStruct();
	
	UFUNCTION()
	void ListenEventPackageSync(const FEventPackageStruct& Package);

	UFUNCTION()
	void ListenEventPackageAsync(const FEventPackageStruct& Package);

	UFUNCTION()
	void Enqueue(const FEventPackageStruct& Package);

	UFUNCTION()
	void CheckQueue();

	UFUNCTION()
	void Execute(const FEventPackageStruct& Package);

	UFUNCTION(BlueprintCallable)
	void	Clear(const int Index);
	
	const FEventPackageStruct* FindPackageByIndex(int Index) const;
	//***********************************************************************

	//***************************本地效果执行*******************************
	void HandleGameStart(const FEventPackageStruct& Package);
	void HandleGameEnd(const FEventPackageStruct& Package);
	void HandleTurnStart(const FEventPackageStruct& Package);
	void HandleTurnEnd(const FEventPackageStruct& Package);
	void HandleCardMove(const FEventPackageStruct& Package);
	void HandleCardAttach(const FEventPackageStruct& Package);
	void HandleCardAttack(const FEventPackageStruct& Package);
	void HandleCardApplyDamage(const FEventPackageStruct& Package);
	void HandleCardActivate(const FEventPackageStruct& Package);
	void HandleCardUpdate(const FEventPackageStruct& Package);
	void HandleCardApplyEffect(const FEventPackageStruct& Package);
	void HandleCardReveal(const FEventPackageStruct& Package);
	//***********************************************************************

	//***************************本地效果管理*******************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)	//所有卡牌，生成卡牌并传入对应位置，便于检索
	TArray<ACardModel*> AllCardModels = {};
	
	UFUNCTION()
	void GetCardsByZone(EZone Zone, int PlayerIndex, TArray<ACardModel*>& OutCards) const;

	UFUNCTION()
	void RefreshZone(EZone Zone, int PlayerIndex);

	UFUNCTION()
	void AddCardToZone(ACardModel* CardModel, EZone NewZone);

	UFUNCTION()
	void RemoveCardFromZone(ACardModel* CardModel);
	
	UFUNCTION()
	void DrawCard(const FEventPackageStruct& Package);

	UFUNCTION()
	void SummonServant(const FEventPackageStruct& Package);

	UFUNCTION()
	void CastSpell(const FEventPackageStruct& Package);

	UFUNCTION()
	void Disappear(const FEventPackageStruct& Package);
	
	UFUNCTION()
	ACardModel* CreateCardModel(const FCardStruct& CardStruct);

	UFUNCTION()
	ACardModel* FindCardModel(const int CardIndex);
	//***********************************************************************
};
