#include "EntityMoveStep.hh"

#include "state/State.hh"
#include "logger/Logger.hh"

namespace octopus
{
void EntityMoveStep::apply(State &state_p) const
{
	Logger::getDebug() << "EntityMoveStep :: " << state_p.getEntity(this->_handle)->_pos << " + "<<this->_move<<std::endl;
	state_p.getEntity(this->_handle)->_pos += this->_move;
}

void EntityMoveStep::revert(State &state_p) const
{
	Logger::getDebug() << "EntityMoveStep :: " << state_p.getEntity(this->_handle)->_pos << " - "<<this->_move<<std::endl;
	state_p.getEntity(this->_handle)->_pos -= this->_move;
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
	double length_l = length(move_l);
	if(length_l > 1e-3)
	{
		move_l /=  length_l;
	}
	move_l *= stepSpeed_p;

	return EntityMoveStep { ent_p._handle, move_l };
}

bool isEntityMoveStepNoOp(EntityMoveStep const &step_p)
{
	return square_length(step_p._move) < 1e-3;
}

} // namespace octopus

