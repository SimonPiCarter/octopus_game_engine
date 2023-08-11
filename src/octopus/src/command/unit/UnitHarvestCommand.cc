
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
#include "step/entity/EntityFrozenStep.hh"
#include "step/resource/ResourceSlotStep.hh"
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
	, _data(_resource, Handle(0), _subMoveCommand.getFinalPoint(), _subMoveCommand.getGridStatus(), _subMoveCommand.getWaypoints())
{}

void UnitHarvestCommand::registerCommand(Step &step_p, State const &state_p)
{
	Resource const *res_l = dynamic_cast<Resource const *>(state_p.getEntity(_resource));
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));

	// check that unit can harvest this resource type
	if(unit_l->_unitModel._maxQuantity.find(res_l->getType()) != unit_l->_unitModel._maxQuantity.end())
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
	return !state_p.isEntityAlive(resource_p) || res_l->_resource <= 1e-5;
}

bool hasResourceToDrop(Unit const * unit_p)
{
	return unit_p->_quantityOfResource > 1e-5;
}

bool isFull(State const &state_p, Unit const * unit_p, Handle const &res_p)
{
	Resource const *res_l = dynamic_cast<Resource const *>(state_p.getEntity(res_p));
	return unit_p->_quantityOfResource >= unit_p->_unitModel._maxQuantity.at(res_l->getType()) - 1e-5;
}

bool inRange(State const &state_p, Unit const * unit_p, Handle const res_p)
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

bool inRange(Unit const &unit_p, Resource const &res_p, int slot_p)
{
	Vector point_l = res_p._harvestPoints.at(slot_p).point;
	return length(point_l - unit_p._pos) < Fixed(0.1);
}

bool isHarvestPointFree(Resource const &res_p, int idx_p, Step const & step_p)
{
	return res_p._harvestPoints.at(idx_p).free && !step_p.isSlotTaken(res_p._handle, idx_p);
}

bool isLockedAlready(Unit const &unit_p, Resource const &res_p, int idx_p)
{
	return !res_p._harvestPoints.at(idx_p).free && res_p._harvestPoints.at(idx_p).harvester == unit_p._handle;
}

bool UnitHarvestCommand::applyCommand(Step & step_p, State const &state_p, CommandData const *data_p, PathManager &pathManager_p) const
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
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, {newRes_l->_pos}));
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
		// in range if not harvest point and in range of resource
		bool inRangeNoPoint_l = inRange(state_p, unit_l, data_l._resource) && data_l._idxSlot < 0 && !hasHarvestPoint(*res_l);
		bool inRangePoint_l = hasHarvestPoint(*res_l) && data_l._idxSlot >= 0 && inRange(*unit_l, *res_l, data_l._idxSlot);

		// need to set up harvest point
		if(hasHarvestPoint(*res_l) && data_l._idxSlot < 0)
		{
			int harvestPoint_l = getBestHarvestPoint(state_p, step_p, *unit_l, *res_l);
			if(harvestPoint_l >= 0)
			{
				step_p.addSteppable(new CommandHarvestPointChangeStep(_handleCommand, data_l._idxSlot, harvestPoint_l));

				Vector target_l = res_l->_harvestPoints.at(harvestPoint_l).point;
				// update move to resource
				step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, {target_l}));
				step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
			}
		}
		bool isHarvestPointFreeOrLocked_l = true;
		// check if the harvest point is free or locked by this entity
		if(inRangePoint_l)
		{
			bool isHarvestPointFree_l = isHarvestPointFree(*res_l, data_l._idxSlot, step_p);
			// true if the resource point is locked by this unit already
			bool lockedByThisAlready_l = isLockedAlready(*unit_l, *res_l, data_l._idxSlot);

			isHarvestPointFreeOrLocked_l = isHarvestPointFree_l || lockedByThisAlready_l;
			// if not free change harvest point
			if(!isHarvestPointFreeOrLocked_l)
			{
				int harvestPoint_l = getBestHarvestPoint(state_p, step_p, *unit_l, *res_l);
				if(harvestPoint_l >= 0)
				{
					step_p.addSteppable(new CommandHarvestPointChangeStep(_handleCommand, data_l._idxSlot, harvestPoint_l));

					Vector target_l = res_l->_harvestPoints.at(harvestPoint_l).point;
					// update move to resource
					step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, {target_l}));
					step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
				}
			}
			// else lock the point
			else if(!lockedByThisAlready_l)
			{
				step_p.addSteppable(new ResourceSlotStep(data_l._resource, data_l._idxSlot, _source, false));
			}
		}
		Logger::getDebug() << "UnitHarvestCommand:: inRangeNoPoint = "<<inRangeNoPoint_l<<std::endl;
		Logger::getDebug() << "UnitHarvestCommand:: inRangePoint = "<<inRangePoint_l<<std::endl;
		Logger::getDebug() << "UnitHarvestCommand:: isHarvestPointFreeOrLocked = "<<isHarvestPointFreeOrLocked_l<<std::endl;

		if(inRangeNoPoint_l || (inRangePoint_l && isHarvestPointFreeOrLocked_l))
		{
			Logger::getDebug() << "UnitHarvestCommand:: gather"<<std::endl;
			// If != type of resource we reset the unit resource gather info
			if (unit_l->_typeOfResource != res_l->getType())
			{
				step_p.addSteppable(new UnitHarvestTypeStep(_source, unit_l->_quantityOfResource, unit_l->_typeOfResource, res_l->getType()));
			}
			if(data_l._timeSinceHarvest + 1.00001 >= unit_l->_unitModel._timeToHarvest)
			{
				step_p.addSteppable(new UnitHarvestQuantityStep(_source, data_l._resource, 1.));
				step_p.addSteppable(new CommandHarvestTimeSinceHarvestStep(_source, data_l._timeSinceHarvest, 0.));
			}
			else
			{
				step_p.addSteppable(new CommandHarvestTimeSinceHarvestStep(_source, data_l._timeSinceHarvest, data_l._timeSinceHarvest+1.));
			}
		}
		else
		{
			Logger::getDebug() << "UnitHarvestCommand:: move to resource"<<std::endl;
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p, pathManager_p);
		}
		return false;
	}

	// if still harvesting or deposit is not available
	if(data_l._harvesting || !state_p.isEntityAlive(data_l._deposit))
	{
		Logger::getDebug() << "UnitHarvestCommand:: look for deposit"<<std::endl;
		if(data_l._harvesting)
		{
			step_p.addSteppable(new CommandHarvestingChangeStep(_handleCommand, data_l._harvesting, false));
			if(data_l._idxSlot >= 0)
			{
				// unlock resource point
				step_p.addSteppable(new ResourceSlotStep(data_l._resource, data_l._idxSlot, _source, true));
			}
		}
		Entity const * deposit_l = lookUpDeposit(state_p, _source, data_l._resource);
		if(deposit_l)
		{
			Logger::getDebug() << "UnitHarvestCommand:: deposit found "<<deposit_l->_handle<<std::endl;
			// update move
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, {deposit_l->_pos}));
			step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
			// update deposit
			step_p.addSteppable(new CommandDepositChangeStep(_handleCommand, data_l._deposit, deposit_l->_handle));
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

			Vector target_l = res_l->_pos;
			int harvestPoint_l = getBestHarvestPoint(state_p, step_p, *unit_l, *res_l);
			if(harvestPoint_l >= 0)
			{
				target_l = res_l->_harvestPoints.at(harvestPoint_l).point;
				step_p.addSteppable(new CommandHarvestPointChangeStep(_handleCommand, data_l._idxSlot, harvestPoint_l));
			}

			// update move back to resource
			step_p.addSteppable(new CommandDataWaypointSetStep(_handleCommand, data_l._waypoints, {target_l}));
			step_p.addSteppable(new CommandMoveUpdateStep(_handleCommand, data_l._stepSinceUpdate, data_l._gridStatus, state_p.getPathGridStatus()));
		}
		else
		{
			Logger::getDebug() << "UnitHarvestCommand:: move to deposit"<<std::endl;
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p, pathManager_p);
		}
	}

	return false;
}

void UnitHarvestCommand::cleanUp(Step & step_p, State const &state_p, CommandData const *data_p) const
{
	_subMoveCommand.cleanUp(step_p, state_p, data_p);

	// unlock resource point
	HarvestMoveData const &data_l = *static_cast<HarvestMoveData const *>(data_p);
	Unit const * unit_l = dynamic_cast<Unit const *>(state_p.getEntity(_source));
	Resource const * res_l = dynamic_cast<Resource const *>(state_p.getEntity(data_l._resource));

	if(data_l._idxSlot >= 0 && isLockedAlready(*unit_l, *res_l, data_l._idxSlot))
	{
		step_p.addSteppable(new ResourceSlotStep(data_l._resource, data_l._idxSlot, _source, true));
	}
}

} // namespace octopus

