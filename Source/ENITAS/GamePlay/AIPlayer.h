#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Client/EventListener.h"
#include "AIPlayer.generated.h"

UCLASS(Blueprintable, BlueprintType)
class AAIPlayer : public AAIController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	
	//****************************本地前置效果*******************************	
	UFUNCTION()
	void FindLocalDeck();
	
	UFUNCTION(Server, Reliable)
	void SendLocalDeck(const TArray<int>& Deck);	//发送牌组到服务器
	//***********************************************************************

	//****************************事件接收器*******************************
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UEventListener> EventListenerClass = UEventListener::StaticClass();
	
	UPROPERTY(BlueprintReadOnly)
	UEventListener* EventListener;
	//***********************************************************************

	//**************************客户端行动请求*****************************
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestEndTurn();
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestPlayCard(const int SourceCard, const int TargetCard, const TArray<int>& Sacrifice);	//源卡牌、目标卡牌（可以为空）、代价
	
	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestAttack(const int SourceCard, const int TargetCard);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestActivate(const int Card, const TArray<int>& Sacrifice);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void RequestTarget(const int PickedTarget);
	//***********************************************************************
};
