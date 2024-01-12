
#include "UnitDropCommand.hh"

#include "command/data/MoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Building.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/CommandDataWaypointStep.hh"
#include "step/command/CommandMoveUpdateStep.hh"
#include "step/unit/UnitHarvestStep.hh"
#include "utils/Box.hh"

namespace octopus
{

UnitDropCommand::UnitDropCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &deposit_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _deposit(deposit_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p, init_p)
{}

void UnitDropCommand::registerCommand(Step &step_p, State const &state_p)
{
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));
	Building const * building_l = dynamic_cast<Building const *>(state_p.getEntity(_deposit));

	// check that unit has resource to drop
	// and player match
	if(unit_l && building_l
	&& unit_l->_quantityOfResource > 0
	&& building_l->_player == unit_l->_player
	&& building_l->_buildingModel.isDeposit(unit_l->_typeOfResource) )
	{
		step_p.addSteppable(state_p, new CommandSpawnStep(this));
	}
	else
	{
		step_p.addSteppable(state_p, new CommandStorageStep(this));
	}
}

bool depostInRange(State const &state_p, Unit const * unit_p, Handle const res_p)
{
	Entity const * ent_l = state_p.getEntity(res_p);
	Box<Fixed> boxA_l { ent_l->_pos.x - ent_l->_model._ray, ent_l->_pos.x + ent_l->_model._ray,
						ent_l->_pos.y - ent_l->_model._ray, ent_l->_pos.y + ent_l->_model._ray };
	Box<Fixed> boxB_l { unit_p->_pos.x - unit_p->_model._ray, unit_p->_pos.x + unit_p->_model._ray,
						unit_p->_pos.y - unit_p->_model._ray, unit_p->_pos.y + unit_p->_model._ray };

	// Use 0.1 as a margin for range resource check
	Box<Fixed> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX)-0.1,
						std::min(boxA_l._upperX, boxB_l._upperX),
						std::max(boxA_l._lowerY, boxB_l._lowerY)-0.1,
						std::min(boxA_l._upperY, boxB_l._upperY) };
	// Check intersections
	if(intersect_l._lowerX < intersect_l._upperX
	&& intersect_l._lowerY < intersect_l._upperY)
	{
		return true;
	}
	return false;
}

bool UnitDropCommand::applyCommand(StepShallow & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
{
	Logger::getDebug() << "UnitDropCommand:: apply Command "<<_source <<std::endl;
	MoveData const &data_l = *static_cast<MoveData const *>(data_p);

	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));

	// if deposit died stop command
	if(!state_p.isEntityAlive(_deposit))
	{
		return true;
	}

	Building const * deposit_l = dynamic_cast<Building const *>(state_p.getEntity(_deposit));
	// else on the way to deposit
	if(depostInRange(state_p, unit_l, _deposit))
	{
		Logger::getDebug() << "UnitDropCommand:: drop"<<std::endl;

		step_p.addSteppable(new UnitHarvestDropStep(_source, unit_l->_quantityOfResource, unit_l->_quantityOfResource * deposit_l->getHarvest()));

		return true;
	}
	else
	{
		Logger::getDebug() << "UnitDropCommand:: move to deposit"<<std::endl;
		// apply move
		_subMoveCommand.applyCommand(step_p, state_p, data_p, pathManager_p);
	}

	return false;
}

} // namespace octopus

