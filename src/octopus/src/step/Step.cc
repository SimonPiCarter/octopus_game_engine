#include "Step.hh"

#include "state/State.hh"
#include "step/entity/EntityMoveStep.hh"
#include "utils/Vector.hh"

#include "StepAdditionVisitor.hh"

namespace octopus
{


Step::~Step()
{
	for(Steppable * step_l : _listSteppable)
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
	StepAdditionVisitor vis_l(*this);
	vis_l(step_p);
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

double & Step::getResourceSpent(unsigned long player_p, ResourceType res_p)
{
	return _spent[player_p][res_p];
}
double Step::getResourceSpent(unsigned long player_p, ResourceType res_p) const
{
	auto &&itPlayer_l = _spent.find(player_p);
	if(itPlayer_l == _spent.end())
	{
		return 0;
	}
	auto &&itRes_l = itPlayer_l->second.find(res_p);
	if(itRes_l == itPlayer_l->second.end())
	{
		return 0;
	}
	return itRes_l->second;
}

const std::map<ResourceType, double> & Step::getResourceSpent(unsigned long player_p) const
{
	/// @brief static empty map when no resources spent for player
	static std::map<ResourceType, double> empty_l;
	auto &&itPlayer_l = _spent.find(player_p);
	if(itPlayer_l == _spent.end())
	{
		return empty_l;
	}
	return itPlayer_l->second;
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
			delete *it_l;
			it_l = step_p.getSteppable().erase(it_l);
		}
		else
		{
			++it_l;
		}
	}
}

void visitAll(Step const &step_p, SteppableVisitor &visitor_p)
{
	for(Steppable const * steppable_l : step_p.getSteppable())
	{
		visitor_p(steppable_l);
	}
}

} // namespace octopus
