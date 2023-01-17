#include "UnitHarvestStep.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Unit.hh"
#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void UnitHarvestQuantityStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestQuantityStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource += _qty;
}
void UnitHarvestQuantityStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestQuantityStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource -= _qty;
}

bool UnitHarvestQuantityStep::isNoOp() const
{
	return std::abs(_qty) < 1e-3;
}

void UnitHarvestTypeStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestTypeStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_typeOfResource = _new;
}
void UnitHarvestTypeStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestTypeStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_typeOfResource = _old;
}

bool UnitHarvestTypeStep::isNoOp() const
{
	return _old == _new;
}

void UnitHarvestDropStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestDropStep :: apply " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource -= _qty;
}

void UnitHarvestDropStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "UnitHarvestDropStep :: revert " << this->_handle <<std::endl;
	Unit * unit_l = dynamic_cast<Unit *>(ent_l);
	unit_l->_quantityOfResource += _qty;
}

bool UnitHarvestDropStep::isNoOp() const
{
	return std::abs(_qty) < 1e-3;
}

} // namespace octopus
