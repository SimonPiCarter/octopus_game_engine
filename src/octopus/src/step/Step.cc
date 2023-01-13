#include "Step.hh"

#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "utils/Vector.hh"

namespace octopus
{


Step::~Step()
{
	for(EntityMoveStep * step_l : _listEntityMoveStep)
	{
		delete step_l;
	}
}

void Step::addEntityMoveStep(EntityMoveStep * step_p)
{
	_listEntityMoveStep.push_back(step_p);
	_listSteppable.push_back(step_p);
}

void Step::addSteppable(Steppable * step_p)
{
	_listSteppable.push_back(step_p);
}

std::list<EntityMoveStep *> &Step::getEntityMoveStep()
{
	return _listEntityMoveStep;
}

std::list<EntityMoveStep *> const &Step::getEntityMoveStep() const
{
	return _listEntityMoveStep;
}

std::list<Steppable *> &Step::getSteppable()
{
	return _listSteppable;
}

std::list<Steppable *> const &Step::getSteppable() const
{
	return _listSteppable;
}

void apply(Step const & step_p, State &state_p)
{
	// apply all steppables
	for(Steppable *steppable_l: step_p.getSteppable())
	{
		steppable_l->apply(state_p);
	}
}

void revert(Step const & step_p, State &state_p)
{
	// apply all steppables (in reverse order)
	for(auto it_l = step_p.getSteppable().rbegin(); it_l != step_p.getSteppable().rend() ; ++it_l)
	{
		(*it_l)->revert(state_p);
	}
}

void compact(Step & step_p)
{
	// remove no op steppables (warning only remove in global list)
	for(auto it_l = step_p.getSteppable().begin(); it_l != step_p.getSteppable().end() ; )
	{
		// remove if no op
		if((*it_l)->isNoOp())
		{
			it_l = step_p.getSteppable().erase(it_l);
		}
		else
		{
			++it_l;
		}
	}
}

} // namespace octopus
