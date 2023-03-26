#include "UnitHarvestStep.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/entity/Resource.hh"
#include "state/player/Player.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void updateFromResource(State &state_p, Resource *res_p)
{
	if(res_p->_resource < 1e-3)
	{
		if(res_p->_alive)
		{
			updateGrid(state_p, res_p, false);
			updateVisionGrid(state_p, res_p, false);
		}
		res_p->_alive = false;
	}
	else
	{
		if(!res_p->_alive)
		{
			updateGrid(state_p, res_p, true);
			updateVisionGrid(state_p, res_p, true);
		}
		res_p->_alive = true;
	}
}

void UnitHarvestQuantityStep::apply(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "UnitHarvestQuantityStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(state_p.getEntity(this->_handle));
	unit_l->_quantityOfResource += _qty;

	Resource * res_l = dynamic_cast<Resource *>(state_p.getEntity(this->_res));
	res_l->_resource -= _qty;

	updateFromResource(state_p, res_l);
}
void UnitHarvestQuantityStep::revert(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "UnitHarvestQuantityStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(state_p.getEntity(this->_handle));
	unit_l->_quantityOfResource -= _qty;

	Resource * res_l = dynamic_cast<Resource *>(state_p.getEntity(this->_res));
	res_l->_resource -= _qty;

	updateFromResource(state_p, res_l);
}

bool UnitHarvestQuantityStep::isNoOp() const
{
	return std::abs(_qty) < 1e-3;
}

void UnitHarvestTypeStep::apply(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestTypeStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_typeOfResource = _new;
	unit_l->_quantityOfResource = 0.;
}
void UnitHarvestTypeStep::revert(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestTypeStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource = _oldVal;
	unit_l->_typeOfResource = _old;
}

bool UnitHarvestTypeStep::isNoOp() const
{
	return _old == _new;
}

void UnitHarvestDropStep::apply(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestDropStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource -= _qty;
	state_p.getPlayer(unit_l->_player)->_resources[unit_l->_typeOfResource] += _dropped;
}

void UnitHarvestDropStep::revert(State &state_p, SteppableData *) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestDropStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource += _qty;
	state_p.getPlayer(unit_l->_player)->_resources[unit_l->_typeOfResource] -= _dropped;
}

bool UnitHarvestDropStep::isNoOp() const
{
	return std::abs(_qty) < 1e-3;
}

} // namespace octopus
