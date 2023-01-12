#include "EntityStep.hh"

namespace octopus
{
EntityStep createEntityStep(Entity &ent_p)
{
	return EntityStep { &ent_p, ent_p._pos };
}

EntityStep createEntityStep(Entity &ent_p, Vector const &target_p, double stepSpeed_p)
{
	Vector move_l = target_p - ent_p._pos;
	double length_l = length(move_l);
	if(length_l > 1e-3)
	{
		move_l /=  length_l;
	}
	move_l *= stepSpeed_p;

	return EntityStep { &ent_p, ent_p._pos + move_l };
}

bool isEntityStepNoOp(EntityStep const &step_p)
{
	return square_length(step_p._ent->_pos - step_p._newPosition) < 1e-3;
}

} // namespace octopus

