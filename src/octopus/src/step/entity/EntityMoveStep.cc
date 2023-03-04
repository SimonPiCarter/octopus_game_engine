#include "EntityMoveStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{

void EntityMoveStep::apply(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityMoveStep :: apply "<< this->_handle << " " << ent_l->_pos << " + "<<this->_move<<std::endl;

	updateGrid(state_p, ent_l, false);
	ent_l->_pos += this->_move;
	updateGrid(state_p, ent_l, true);
	updateExplorationGrid(state_p, ent_l, true);

}

void EntityMoveStep::revert(State &state_p) const
{
	Entity * ent_l = state_p.getEntity(this->_handle);
	Logger::getDebug() << "EntityMoveStep :: revert " << this->_handle << " " << ent_l->_pos << " - "<<this->_move<<std::endl;

	updateExplorationGrid(state_p, ent_l, false);
	updateGrid(state_p, ent_l, false);
	ent_l->_pos -= this->_move;
	updateGrid(state_p, ent_l, true);
}

bool EntityMoveStep::isNoOp() const
{
	return isEntityMoveStepNoOp(*this);
}

EntityMoveStep createEntityMoveStep(Entity const &ent_p)
{
	return EntityMoveStep { ent_p._handle, {0., 0.} };
}

EntityMoveStep createEntityMoveStep(Entity const &ent_p, Vector const &target_p, double stepSpeed_p)
{
	Vector move_l = target_p - ent_p._pos;
	Fixed length_l = length(move_l);
	if(length_l > 1e-3)
	{
		move_l /=  length_l;
	}
	move_l *= stepSpeed_p;

	return EntityMoveStep { ent_p._handle, move_l };
}

bool isEntityMoveStepNoOp(EntityMoveStep const &step_p)
{
	return is_zero(step_p._move);
}

} // namespace octopus

