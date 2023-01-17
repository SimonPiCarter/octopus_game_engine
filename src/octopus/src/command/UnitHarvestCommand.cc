
#include "UnitHarvestCommand.hh"

#include "command/data/AttackMoveData.hh"
#include "logger/Logger.hh"
#include "state/State.hh"
#include "step/Step.hh"
#include "step/command/harvest/CommandHarvestStep.hh"

namespace octopus
{

UnitHarvestCommand::UnitHarvestCommand(Handle const &commandHandle_p, Handle const &source_p, Handle const &resource_p,
		Vector const &finalPoint_p, unsigned long gridStatus_p, std::list<Vector> const &waypoints_p)
	: Command(commandHandle_p)
	, _source(source_p)
	, _resource(resource_p)
	, _subMoveCommand(commandHandle_p, source_p, finalPoint_p, gridStatus_p, waypoints_p)
{}

bool resourceExhausted(State const state_p, Handle const &resource_p)
{
	Resource const * res_l = dynamic_cast<Resource const *>(state_p.getEntity(resource_p));
	return res_l->_resource <= 1e-3;
}

bool isFull(Unit const * unit_p)
{
	return unit_p->_quantityOfResource >= 10.;
}

bool inRange(State const &state_p, Unit const * unit_p, Handle const res_p)
{
	Entity const * ent_l = state_p.getEntity(res_p);
	Box<double> boxA_l { ent_l->_pos.x - ent_l->_model._ray, ent_l->_pos.x + ent_l->_model._ray,
						ent_l->_pos.y - ent_l->_model._ray, ent_l->_pos.y + ent_l->_model._ray };
	Box<double> boxB_l { unit_p->_pos.x - unit_p->_model._ray, unit_p->_pos.x + unit_p->_model._ray,
						unit_p->_pos.y - unit_p->_model._ray, unit_p->_pos.y + unit_p->_model._ray };

	Box<double> intersect_l = { std::max(boxA_l._lowerX, boxB_l._lowerX),
						std::min(boxA_l._upperX, boxB_l._upperX),
						std::max(boxA_l._lowerY, boxB_l._lowerY),
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
	HarvestMoveData const &data_l = *static_cast<HarvestMoveData const *>(data_p);
	Unit const * unit_l = static_cast<Unit const *>(state_p.getEntity(_source));
	// Is resource exhausted
	if(resourceExhausted(state_p, data_l._resource))
	{
		// Look up for new target
		Entity const * newRes_l = lookUpNewResource(state_p, _source, data_l._resource);
		if(newRes_l)
		{
			// update resource in steps
			step_p.addSteppable(new CommandResourceChangeStep(_commandableHandle, data_l._resource, newRes_l->_handle));
			return false;
		}
		else if(!hasResourceToDrop(unit_l))
		{
			// no resource to drop -> command is over
			return true;
		}
	}
	else if(!isFull(unit_l))
	{
		if(inRange(state_p, unit_l, data_l._resource))
		{
			/// @todo change gather rate to read data
			step_p.addSteppable(new UnitHarvestQuantityStep(_source, 1.)));
		}
		else
		{
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p);
		}
		return false;
	}

	// if still harvesting
	if(data_l._harvesting)
	{
		Entity const * deposit_l = lookUpDeposit(state_p, _source, data_l._resource);
		if(deposit_l)
		{
			// update deposit
			step_p.addSteppable(new CommandDepositChangeStep(_commandableHandle, data_l._deposit, deposit_l->_handle));
		}
		else
		{
			return true;
		}
	}
	else
	{
		// else on the way to deposit
		if(inRange(state_p, unit_l, data_l._deposit))
		{
			step_p.addSteppable(new UnitHarvestDropStep(_source, unit_l->_quantityOfResource)));
		}
		else
		{
			// apply move
			_subMoveCommand.applyCommand(step_p, state_p, data_p);
		}
	}

	return false;
}

} // namespace octopus

