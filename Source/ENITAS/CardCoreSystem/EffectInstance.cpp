#include "../CardCoreSystem/EffectInstance.h"
#include "../CardCoreSystem/EffectContext.h"
#include "../CardCoreSystem/CardCoreDriver.h"
#include "../CardCoreSystem/CardInstance.h"

bool UEffectInstance::CheckCondition_Effect_Implementation(UEffectContext* OuterContext)
{
	return true;	//蓝图重定义
}

bool UEffectInstance::CheckTargets_Effect_Implementation(ACardCoreDriver* OuterDriver)
{
	return true;	//蓝图重定义
}

TArray<int> UEffectInstance::GetValidTargets_Implementation()
{
	return {};	//基于第N个对象的要求返回合法对象列表。蓝图重定义
}

void UEffectInstance::Activate()
{
	if (CheckTargets_Effect(Driver))
	{
		if (TargetNum == 1 && TargetPicked.IsEmpty() && StoredContext -> PrePickUp)
		{
			TargetPicked.Emplace(StoredContext -> PrePickUp);	//即时补充预先选择的对象
		}
		if (TargetNum != 0 && TargetNum != TargetPicked.Num())
		{
			Require();
		}
		else
		{
			Execute();	//正常结算
		}
	}
	else
	{
		Complete();	//跳过结算
	}
}

void UEffectInstance::Require()
{
	if (Driver)
	{
		Driver -> RequireForTarget(CardInstance -> CardStruct.PlayerIndex, GetValidTargets());	//通过Driver请求目标
	}
}

void UEffectInstance::CallBack(UCardInstance* PickedTarget)
{
	TargetPicked.Emplace(PickedTarget);	//补充对象列表
	Activate();
}

void UEffectInstance::Execute()
{
	//效果执行的主体，直接操纵CardInstance
	Complete();
	Notify();
}

void UEffectInstance::Notify() const
{
	switch (StoredContext -> Condition)
	{
	case ECondition::BattleCry :
		{
			UEffectContextForOnBattleCry* MulticastContext = NewObject<UEffectContextForOnBattleCry>(Driver);
			MulticastContext -> Condition = ECondition::OnBattleCry;
			MulticastContext -> SourceCard = CardInstance;
			Driver -> CreateNewEffectForConditionEliminate(CardInstance, MulticastContext);
			break;
		}
	case ECondition::DeathRattle :
		{
			UEffectContextForOnDeathRattle* MulticastContext = NewObject<UEffectContextForOnDeathRattle>(Driver);
			MulticastContext -> Condition = ECondition::OnDeathRattle;
			MulticastContext -> SourceCard = CardInstance;
			Driver -> CreateNewEffectForConditionEliminate(CardInstance, MulticastContext);
			break;
		}
		default : break;
	}
}

void UEffectInstance::Complete() const
{
	Driver  -> SolveStack();
}



