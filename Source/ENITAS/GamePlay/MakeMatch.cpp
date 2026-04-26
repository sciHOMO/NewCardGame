#include "../Gameplay/MakeMatch.h"
#include "../GamePlay/CardPlayer.h"
#include "../GamePlay/CardPlayerInfo.h"
#include "../CardCoreSystem/CardCoreDriver.h"
#include "GameFramework/PlayerState.h"
#include "Runtime/AIModule/Classes/AIController.h"

AMakeMatch::AMakeMatch()
{
	GameSessionClass = nullptr;
	GameStateClass = ACardCoreDriver::StaticClass();
	PlayerControllerClass = ACardPlayer::StaticClass();
	PlayerStateClass = ACardPlayerInfo::StaticClass();
	HUDClass = nullptr;
	DefaultPawnClass = nullptr;
}

void AMakeMatch::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	NewPlayer -> PlayerState -> SetPlayerId(LoginPlayerNum++);
	
	if (const ENetMode NetMode = GetWorld()->GetNetMode(); NetMode == NM_Standalone && LoginPlayerNum)
	{
		CreateAIPlayer();	//单机模式立刻创建本地机器人
	}
}

void AMakeMatch::CreateAIPlayer()
{
	if (AIPlayerClass)
	{
		if (AAIController* AIPlayer = GetWorld() -> SpawnActor<AAIController>(AIPlayerClass))
		{
			if (PlayerStateClass)
			{
				if (APlayerState* AIPlayerInfo = GetWorld() -> SpawnActor<APlayerState>(PlayerStateClass))
				{
					AIPlayerInfo -> SetPlayerId(LoginPlayerNum++);	//创建本地机器人并设置PS
					AIPlayerInfo -> SetIsABot(true);
					AIPlayer -> PlayerState = AIPlayerInfo;
				}
			}
		}
	}
}


