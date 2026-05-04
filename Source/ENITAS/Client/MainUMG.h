#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUMG.generated.h"

/*
对应每个需要UMG参与的事件，接受并执行。需要接口事件。 
*/

UCLASS()
class UMainUMG : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void NotifyGameStart();

	UFUNCTION(BlueprintNativeEvent)
	void NotifyTurnStart(const int PlayerIndex);

	UFUNCTION(BlueprintNativeEvent)
	void NotifyPickUpSacrifice(const int Level);

	UFUNCTION(BlueprintNativeEvent)
	void NotifyPickUpCount(const int Num);

	/** 打断献祭/目标选择 UI（校验失败或未选对象时调用）*/
	UFUNCTION(BlueprintNativeEvent)
	void NotifySacrificeDismissed();
};
