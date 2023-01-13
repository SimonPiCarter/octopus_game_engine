#include "EntityStep.hh"

#include "state/State.hh"

namespace octopus
{
void EntityStep::apply(State &state_p) const
{
	state_p.getEntity(this->_handle)->_pos += this->_move;
}

void EntityStep::revert(State &state_p) const
{
	state_p.getEntity(this->_handle)->_pos -= this->_move;
}

bool EntityStep::isNoOp() const
{
	return isEntityStepNoOp(*this);
}

EntityStep createEntityStep(Entity const &ent_p)
{
	return EntityStep { ent_p._handle, {0., 0.} };
}

EntityStep createEntityStep(Entity const &ent_p, Vector const &target_p, double stepSpeed_p)
{
	Vector move_l = target_p - ent_p._pos;
	double length_l = length(move_l);
	if(length_l > 1e-3)
	{
		move_l /=  length_l;
	}
	move_l *= stepSpeed_p;

	return EntityStep { ent_p._handle, move_l };
}

bool isEntityStepNoOp(EntityStep const &step_p)
{
	return square_length(step_p._move) < 1e-3;
}

} // namespace octopus

