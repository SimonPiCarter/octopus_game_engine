#ifndef __StepAdditionVisitor__
#define __StepAdditionVisitor__

#include "Steppable.hh"

namespace octopus
{
class Step;

/// @brief This class is used to add usefull information in the step when adding a step
class StepAdditionVisitor : public SteppableVisitor
{
public:
	StepAdditionVisitor(Step &step_p);
	virtual ~StepAdditionVisitor() {}

	/// @brief untypped visiting method
	void operator()(Steppable const *steppable_p)
	{
		steppable_p->visit(this);
	}

	virtual void visit(AttackModifierStep const *) {}
	virtual void visit(BuildingSpawnStep const *);
	virtual void visit(BuildingStep const *) {}
	virtual void visit(BuildingCancelStep const *);
	virtual void visit(BuildingRemoveRallyPointStep const *) {}
	virtual void visit(BuildingSetRallyPointStep const *) {}
	virtual void visit(CancelUnitProductionStep const *);
	virtual void visit(CommandAddSubCommandStep const *) {}
	virtual void visit(CommandDataWaypointAddStep const *) {}
	virtual void visit(CommandDataWaypointRemoveStep const *) {}
	virtual void visit(CommandDataWaypointSetStep const *) {}
	virtual void visit(CommandDelSubCommandStep const *) {}
	virtual void visit(CommandDepositChangeStep const *) {}
	virtual void visit(CommandHarvestingChangeStep const *) {}
	virtual void visit(CommandHarvestTimeSinceHarvestStep const *) {}
	virtual void visit(CommandIncrementNoProgressStep const *) {}
	virtual void visit(CommandMoveUpdateStep const *) {}
	virtual void visit(CommandMoveLosStep const *) {}
	virtual void visit(CommandUpdateFlockingReached const *) override {}
	virtual void visit(CommandMoveStepSinceUpdateIncrementStep const *) {}
	virtual void visit(CommandNewTargetStep const *) {}
	virtual void visit(CommandNextStep const *) {}
	virtual void visit(CommandResourceChangeStep const *) {}
	virtual void visit(CommandSetPositionFromStep const *) {}
	virtual void visit(CommandSpawnStep const *) {}
	virtual void visit(CommandStorageStep const *) {}
	virtual void visit(CommandUpdateLastIdStep const *) {}
	virtual void visit(CommandUpdateLastPosStep const *) {}
	virtual void visit(CommandWindUpDiffStep const *) {}
	virtual void visit(EntityAttackStep const *) {}
	virtual void visit(EntityBuffStep const *) {}
	virtual void visit(EntityConditionalBuffStep const *) {}
	virtual void visit(EntityFrozenStep const *) {}
	virtual void visit(EntityHitPointChangeStep const *);
	virtual void visit(EntityMoveStep const *) {}
	virtual void visit(EntityUpdateReloadAbilityStep const *) override;
	virtual void visit(EntityUpdateWaitingStep const *) {}
	virtual void visit(EntitySpawnStep const *);
	virtual void visit(FlyingCommandSpawnStep const *);
	virtual void visit(FlyingCommandPopStep const *) {}
	virtual void visit(MissingResourceStep const *) {}
	virtual void visit(PlayerAddBuildingModel const *) {}
	virtual void visit(PlayerAddCostBonusStep const *) {}
	virtual void visit(PlayerAddOptionStep const *) {}
	virtual void visit(PlayerAddTimeProdBonusStep const *) {}
	virtual void visit(PlayerAttackModAllStep const *) {}
	virtual void visit(PlayerBuffAllStep const *) {}
	virtual void visit(PlayerConditionalBuffAllStep const *) {}
	virtual void visit(PlayerLevelUpUpgradeStep const *) {}
	virtual void visit(PlayerPopOptionStep const *) {}
	virtual void visit(PlayerProducedUpgradeStep const *);
	virtual void visit(PlayerSpawnStep const *) {}
	virtual void visit(PlayerSpendResourceStep const *);
	virtual void visit(PlayerUpdateBuildingCountStep const *) override {}
	virtual void visit(ProductionPaidStep const *) {}
	virtual void visit(ProductionProgressionStep const *) {}
	virtual void visit(ResourceSlotStep const *) override;
	virtual void visit(ResourceSpawnStep const *);
	virtual void visit(StateAddConstraintPositionStep const *) override {}
	virtual void visit(StateDrawStep const *) override {}
	virtual void visit(StateFreeHandleStep const *) override {}
	virtual void visit(StateRemoveConstraintPositionStep const *) override {}
	virtual void visit(StateTemplePositionAddStep const *) override {}
	virtual void visit(StateTemplePositionRemoveStep const *) {}
	virtual void visit(StateWinStep const *) {}
	virtual void visit(TeamVisionStep const *) {}
	virtual void visit(TickingStep const *) {}
	virtual void visit(TimerDataUpdateStep const *) {}
	virtual void visit(TriggerCountChange const *) {}
	virtual void visit(TriggerEnableChange const *) {}
	virtual void visit(TriggerEntityAddStep const *) {}
	virtual void visit(TriggerEntityResetStep const *) {}
	virtual void visit(TriggerSpawn const *) {}
	virtual void visit(TriggerStepCountChange const *) {}
	virtual void visit(UnitHarvestDropStep const *) {}
	virtual void visit(UnitHarvestQuantityStep const *) {}
	virtual void visit(UnitHarvestTypeStep const *) {}
	virtual void visit(UnitSpawnStep const *);
	virtual void visit(VisionChangeStep const *) {}

private:
	Step &_step;
};

} // namespace octopus

#endif
