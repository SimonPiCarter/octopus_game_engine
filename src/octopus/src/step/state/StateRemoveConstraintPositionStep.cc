#include "StateRemoveConstraintPositionStep.hh"

#include "state/State.hh"
#include "step/ConstraintPosition.hh"

namespace octopus
{


void StateRemoveConstraintPositionStep::apply(State &state_p) const
{
	ConstraintPositionLibrary &lib_l = state_p.getConstraintPositionLibrary();

	if(_x)
	{
		std::list<ConstraintPositionX> &list_l = lib_l.getXConstraintForPlayer(_player);
		for(auto it_l = list_l.begin() ; it_l != list_l.end() ; ++it_l)
        {
            if(it_l->x == _val
            && it_l->minY == _min
            && it_l->maxY == _max
            && it_l->less == _less)
            {
                list_l.erase(it_l);
                break;
            }
        }
	}
	else
	{
		std::list<ConstraintPositionY> &list_l = lib_l.getYConstraintForPlayer(_player);
		for(auto it_l = list_l.begin() ; it_l != list_l.end() ; ++it_l)
        {
            if(it_l->y == _val
            && it_l->minX == _min
            && it_l->maxX == _max
            && it_l->less == _less)
            {
                list_l.erase(it_l);
                break;
            }
        }
	}
}

void StateRemoveConstraintPositionStep::revert(State &state_p, SteppableData const *) const
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

} // namespace octopus
