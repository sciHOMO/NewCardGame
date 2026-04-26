#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MakeMatch.generated.h"

/*
游戏匹配，本质是AGameMode
*/

class AAIController;

UCLASS(Blueprintable, BlueprintType)
class AMakeMatch : public AGameMode
{
	GENERATED_BODY()

	AMakeMatch();

	//玩家登录
	virtual void OnPostLogin(AController* NewPlayer) override;

public:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AAIController> AIPlayerClass = nullptr;	//BOT类

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int LoginPlayerNum = 0;	//登陆玩家数量

	UFUNCTION()
	void CreateAIPlayer();	//创建机器人
};
