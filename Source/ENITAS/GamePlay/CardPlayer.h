#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "../Client/EventListener.h"
#include "CardPlayer.generated.h"

class UMainUMG;
//玩家操作模式枚举型
UENUM(BlueprintType)
enum class EInputMode : uint8
{
	Idle											UMETA(DisplayName = "Idle"),
	PlayCard									UMETA(DisplayName = "PlayCard"),
	AttackOrActivate						UMETA(DisplayName = "AttackOrActivate"),
	Attack										UMETA(DisplayName = "Attack"),
	Activate									UMETA(DisplayName = "Activate"),
	PickUpSacrificesForPlay			UMETA(DisplayName = "PickUpSacrificesForPlay"),
	PickUpSacrificesForEffect		UMETA(DisplayName = "PickUpSacrificesForEffect"),
	PickUpTargets							UMETA(DisplayName = "PickUpTargets"),
};

UCLASS(Blueprintable, BlueprintType)
class ACardPlayer : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
	
public:

	//****************************本地前置效果*******************************	
	UFUNCTION()
	void SetLocalElement();	//准备本地元素UI与摄像机

	UFUNCTION(BlueprintCallable)
	void FindLocalDeck();	//准备本地卡组
	
	UFUNCTION(Server, Reliable)
	void SendLocalDeck(const TArray<int>& Deck);	//发送牌组到服务器，作为FindLocalDeck的回调
	//***********************************************************************

	//****************************事件接收器*******************************
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UEventListener> EventListenerClass = UEventListener::StaticClass();
	
	UPROPERTY(BlueprintReadOnly)
	UEventListener* EventListener = nullptr;
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

	//**************************客户端行动检查*****************************
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UMainUMG> MainUMGClass = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UMainUMG* MainUMG = nullptr;
	//***********************************************************************
	
	//**************************客户端行动检查*****************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	ACardModel* FocusActor = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	ACardModel* SecondFocusActor = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TMap<int, ACardModel*> SacrificeMap = {};

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EInputMode InputMode = EInputMode::Idle;

	UFUNCTION()
	void SetInputMode(EInputMode NewMode);	//伪状态机
	
	UFUNCTION(BlueprintCallable)
	void LeftMouseButtonClicked();

	UFUNCTION(BlueprintCallable)
	void LeftMouseButtonReleased();

	UFUNCTION(BlueprintCallable)
	void CallBackPickUpSacrifice();
	
	UFUNCTION()
	ACardModel* CheckHitResult() const;

	UFUNCTION()
	bool CheckInsideBoard() const;
	//***********************************************************************
};
