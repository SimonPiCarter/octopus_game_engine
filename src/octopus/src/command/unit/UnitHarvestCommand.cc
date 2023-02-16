
#include "UnitHarvestCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/command/harvest/CommandHarvestStep.hh"
#include "step/command/CommandDataWaypointStep.hh"
#include "step/command/CommandMoveUpdateStep.hh"
#include "step/unit/UnitHarvestStep.hh"
#include "utils/Box.hh"

namespace octopus
{

UnitHarvestCommand::UnitHarvestCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &resource_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p, bool init_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _resource(resource_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p, init_p)
{}

void UnitHarvestCommand::registerCommand(Step &step_p, State const &state_p)
{
	Resource const *res_l = dynamic_cast<Resource const *>(state_p.getEntity(_resource));
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));

	if(unit_l->_unitModel._maxQuantity.find(res_l->_type) != unit_l->_unitModel._maxQuantity.end())
	{
		step_p.addSteppable(new CommandSpawnStep(this));
	}
	else
	{
		step_p.addSteppable(new CommandStorageStep(this));
	}
}

bool resourceExhausted(State const &state_p, Handle const &resource_p)
{
	Resource const * res_l = dynamic_cast<Resource const *>(state_p.getEntity(resource_p));
	return res_l->_resource <= 1e-3;
}

bool hasResourceToDrop(Unit const * unit_p)
{
	return unit_p->_quantityOfResource > 1e-3;
}

bool isFull(State const &state_p, Unit const * unit_p, Handle const &res_p)
{
	Resource const *res_l = dynamic_cast<Resource const *>(state_p.getEntity(res_p));
	return unit_p->_quantityOfResource >= unit_p->_unitModel._maxQuantity.at(res_l->_type) - 1e-5;
}

bool inRange(State const &state_p, Unit const * unit_p, Handle const res_p)
{
	Entity const * ent_l = state_p.getEntity(res_p);
	Box<double> boxA_l { ent_l->_pos.x - ent_l->_model._ray, ent_l->_pos.x + ent_l->_model._ray,
						ent_l->_pos.y - ent_l->_model._ray, ent_l->_pos.y + ent_l->_model._ray };
	Box<double> boxB_l { unit_p->_pos.x - unit_p->_model._ray, unit_p->_pos.x + unit_p->_model._ray,
						unit_p->_pos.y - unit_p->_model._ray, unit_p->_pos.y + unit_p->_model._ray };

	// Use 0.1 as a margin for range resource check
	Box<double> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX)-0.1,
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

bool UnitHarvestCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	Logger::getDebug() << "UnitHarvestCommand:: apply Command "<<_source <<std::endl;
	HarvestMoveData const &data_l = *static_cast<HarvestMoveData const *>(data_p);
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));
	Resource const * res_l = dynamic_cast<Resource const *>(state_p.getEntity(data_l._resource));

	bool notFull_l = !isFull(state_p, unit_l, data_l._resource);
	// Is resource exhausted and harvesting
	if(notFull_l && resourceExhausted(state_p, data_l._resource) && data_l._harvesting)
	{
		Logger::getDebug() << "UnitHarvestCommand:: resource exhausted "<<data_l._resource <<std::endl;
		// Look up for new target
		Entity const * newRes_l = lookUpNewResource(state_p, _source, data_l._resource);
		if(newRes_l)
		{
			Logger::getDebug() << "UnitHarvestCommand:: new resource "<<newRes_l->_handle <<std::endl;
			// update move
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, computePath(state_p, _source, newRes_l->_pos, {newRes_l})));
			step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
			// update resource in steps
			step_p.addSteppable(new CommandResourceChangeStep(_handleCommand, data_l._resource, newRes_l->_handle));
			return false;
		}
		else if(!hasResourceToDrop(unit_l))
		{
			Logger::getDebug() << "UnitHarvestCommand:: no new resource found and no resource to drop"<<std::endl;
			// no resource to drop -> command is over
			return true;
		}
		Logger::getDebug() << "UnitHarvestCommand:: no new resource found "<<std::endl;
	}
	else if(notFull_l && data_l._harvesting)
	{
		Logger::getDebug() << "UnitHarvestCommand:: is not full"<<std::endl;
		if(inRange(state_p, unit_l, data_l._resource))
		{
			Logger::getDebug() << "UnitHarvestCommand:: gather"<<std::endl;
			// If != type of resource we reset the unit resource gather info
			if (unit_l->_typeOfResource != res_l->_type)
			{
				step_p.addSteppable(new UnitHarvestTypeStep(_source, data_l._resource, unit_l->_typeOfResource, res_l->_type));
			}
			/// @todo change gather rate to read data
			step_p.addSteppable(new UnitHarvestQuantityStep(_source, data_l._resource, 0.01));
		}
		else
		{
			Logger::getDebug() << "UnitHarvestCommand:: move to resource"<<std::endl;
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p);
		}
		return false;
	}

	// if still harvesting or deposit is not available
	if(data_l._harvesting || !state_p.getEntity(data_l._deposit)->_alive)
	{
		Logger::getDebug() << "UnitHarvestCommand:: look for deposit"<<std::endl;
		Entity const * deposit_l = lookUpDeposit(state_p, _source, data_l._resource);
		if(deposit_l)
		{
			Logger::getDebug() << "UnitHarvestCommand:: deposit found "<<deposit_l->_handle<<std::endl;
			// update move
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, computePath(state_p, _source, deposit_l->_pos, {deposit_l})));
			step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
			// update deposit
			step_p.addSteppable(new CommandDepositChangeStep(_handleCommand, data_l._deposit, deposit_l->_handle));
			step_p.addSteppable(new CommandHarvestingChangeStep(_handleCommand, data_l._harvesting, false));
		}
		else
		{
			Logger::getDebug() << "UnitHarvestCommand:: no deposit found"<<std::endl;
			return true;
		}
	}
	else
	{
		// else on the way to deposit
		if(inRange(state_p, unit_l, data_l._deposit))
		{
			Entity const * ent_l = state_p.getEntity(data_l._deposit);
			Logger::getDebug() << "UnitHarvestCommand:: drop"<<std::endl;
			step_p.addSteppable(new UnitHarvestDropStep(_source, unit_l->_quantityOfResource, unit_l->_quantityOfResource * ent_l->getHarvest()));
			step_p.addSteppable(new CommandHarvestingChangeStep(_handleCommand, data_l._harvesting, true));

			// update move back to resource
			std::list<Vector> path_l = computePath(state_p, _source, res_l->_pos, {res_l});
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, path_l));
			step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
		}
		else
		{
			Logger::getDebug() << "UnitHarvestCommand:: move to deposit"<<std::endl;
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p);
		}
	}

	return false;
}

} // namespace octopus

