#include "StateAddConstraintPositionStep.hh"

#include "state/State.hh"
#include "step/ConstraintPosition.hh"

namespace octopus
{


void StateAddConstraintPositionStep::apply(State &state_p) const
{
	ConstraintPositionLibrary &lib_l = state_p.getConstraintPositionLibrary();

	if(_x)
	{
		std::list<ConstraintPositionX> &list_l = lib_l.getXConstraintForPlayer(_player);
		list_l.push_back({_val, _min, _max, _less});
	}
	else
	{
		std::list<ConstraintPositionY> &list_l = lib_l.getYConstraintForPlayer(_player);
		list_l.push_back({_val, _min, _max, _less});
	}
}

void StateAddConstraintPositionStep::revert(State &state_p, SteppableData const *) const
{
	ConstraintPositionLibrary &lib_l = state_p.getConstraintPositionLibrary();

	if(_x)
	{
		std::list<ConstraintPositionX> &list_l = lib_l.getXConstraintForPlayer(_player);
		list_l.pop_back();
	}
	else
	{
		std::list<ConstraintPositionY> &list_l = lib_l.getYConstraintForPlayer(_player);
		list_l.pop_back();
	}
}

} // namespace octopus
