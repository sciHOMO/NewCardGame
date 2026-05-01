#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "../CardCoreSystem/CardInstance.h"
#include "../CardCoreSystem/EventPackage.h"
#include "GameFramework/Actor.h"
#include "CardModel.generated.h"

struct FEventPackageStruct;
class UEventListener;

//卡牌状态枚举型
UENUM(BlueprintType)
enum class EState : uint8
{
	Anim					UMETA(DisplayName = "Anim"),
	Lerp						UMETA(DisplayName = "Lerp"),
	Follow					UMETA(DisplayName = "Follow"),
	Focus					UMETA(DisplayName = "Focus"),
	Hide					UMETA(DisplayName = "Hide"),
};

UCLASS()
class ACardModel : public AActor
{
	GENERATED_BODY()
	
public:	

	ACardModel();

protected:

	virtual void BeginPlay() override;

public:	

	virtual void Tick(float DeltaTime) override;
	
	//***************************卡牌组件*****************************
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UBoxComponent* CardCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CardMesh;
	//*****************************************************************
	
	//***************************卡牌信息*****************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FCardStruct CardStruct = FCardStruct();

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FEventPackageStruct PackageStruct = FEventPackageStruct();

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UEventListener* EventListener = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EZone CurrentZone = EZone::PlaceHolder;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FVector AdditiveLocation = FVector::ZeroVector;
	//*****************************************************************
	
	//*****************************卡牌活动****************************
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EState CardState = EState::Anim;

	UFUNCTION(BlueprintCallable)
	void SetCardState(EState NewState);	//伪状态机
	
	UFUNCTION()
	void LerpToTargetLocation(const float LerpSpeed);
	
	UFUNCTION(BlueprintNativeEvent)
	void StartDrawCard(const bool Owning, const FEventPackageStruct& Package);

	UFUNCTION(BlueprintCallable)
	void EndDrawCard();

	UFUNCTION(BlueprintNativeEvent)
	void StartSummonServant(const bool Owning, const FEventPackageStruct& Package);

	UFUNCTION(BlueprintCallable)
	void EndSummonServant();

	UFUNCTION(BlueprintNativeEvent)
	void StartCastSpell(const bool Owning, const FEventPackageStruct& Package);

	UFUNCTION(BlueprintCallable)
	void EndCastSpell();

	UFUNCTION(BlueprintNativeEvent)
	void StartDisappear(const bool Owning, const FEventPackageStruct& Package);

	UFUNCTION(BlueprintCallable)
	void EndDisappear();
};
