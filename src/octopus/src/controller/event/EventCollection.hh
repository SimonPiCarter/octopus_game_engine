#ifndef __EventCollection__
#define __EventCollection__

#include <list>
#include <unordered_set>
#include <unordered_map>

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
class EventEntityModelDied;
class EventEntityModelFinished;
class State;

/// @brief store event visiting a step
/// @warning visitor is not stateless therefore create one
/// visitor per step visited
class EventCollection : public SteppableVisitor
{
public:
	EventCollection(State const &state_p);
	~EventCollection();

	std::list<EventEntityModelDied *> _listEventEntityModelDied;
	std::list<EventEntityModelFinished *> _listEventEntityModelFinished;

	// may create event
	virtual void visit(BuildingStep const *);
	virtual void visit(EntityHitPointChangeStep const *);
	virtual void visit(EntitySpawnStep const *);
	virtual void visit(UnitSpawnStep const *);

	/// NA
	virtual void visit(BuildingSpawnStep const *) {}
	virtual void visit(BuildingCancelStep const *) {}
	virtual void visit(CommandAddSubAttackStep const *) {}
	virtual void visit(CommandDataWaypointAddStep const *) {}
	virtual void visit(CommandDataWaypointRemoveStep const *) {}
	virtual void visit(CommandDataWaypointSetStep const *) {}
	virtual void visit(CommandDelSubAttackStep const *) {}
	virtual void visit(CommandDepositChangeStep const *) {}
	virtual void visit(CommandHarvestingChangeStep const *) {}
	virtual void visit(CommandHarvestTimeSinceHarvestStep const *) {}
	virtual void visit(CommandIncrementNoProgressStep const *) {}
	virtual void visit(CommandMoveUpdateStep const *) {}
	virtual void visit(CommandMoveStepSinceUpdateIncrementStep const *) {}
	virtual void visit(CommandNewTargetStep const *) {}
	virtual void visit(CommandNextStep const *) {}
	virtual void visit(CommandResourceChangeStep const *) {}
	virtual void visit(CommandSetPositionFromStep const *) {}
	virtual void visit(CommandSpawnStep const *) {}
	virtual void visit(CommandStorageStep const *) {}
	virtual void visit(CommandUpdateLastPosStep const *) {}
	virtual void visit(CommandWindUpDiffStep const *) {}
	virtual void visit(EntityAttackStep const *) {}
	virtual void visit(EntityBuffStep const *) {}
	virtual void visit(EntityMoveStep const *) {}
	virtual void visit(EntityUpdateWaitingStep const *) {}
	virtual void visit(MissingResourceStep const *) {}
	virtual void visit(PlayerAddBuildingModel const *) {}
	virtual void visit(PlayerAddOptionDivinityStep const *) {}
	virtual void visit(PlayerAnchorDivinityStep const *) {}
	virtual void visit(PlayerLevelUpDivinityStep const *) {}
	virtual void visit(PlayerResetOptionDivinityStep const *) {}
	virtual void visit(PlayerSpawnStep const *) {}
	virtual void visit(PlayerSpendResourceStep const *) {}
	virtual void visit(ProductionPaidStep const *) {}
	virtual void visit(ProductionProgressionStep const *) {}
	virtual void visit(ResourceSpawnStep const *) {}
	virtual void visit(TickingStep const *) {}
	virtual void visit(TriggerCountChange const *) {}
	virtual void visit(TriggerEnableChange const *) {}
	virtual void visit(TriggerEntityAddStep const *) {}
	virtual void visit(TriggerEntityResetStep const *) {}
	virtual void visit(TriggerSpawn const *) {}
	virtual void visit(TriggerStepCountChange const *) {}
	virtual void visit(UnitHarvestDropStep const *) {}
	virtual void visit(UnitHarvestQuantityStep const *) {}
	virtual void visit(UnitHarvestTypeStep const *) {}

private:
	State const &_state;
	/// statefull data
	std::unordered_set<Handle> _finishedHandles;
	std::unordered_map<Handle, double> _buildingProgress;

	std::unordered_set<Handle> _diedHandles;
	std::unordered_map<Handle, double> _hpChange;
};

}

#endif
