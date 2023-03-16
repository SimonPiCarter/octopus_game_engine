#include "Step.hh"

#include "state/State.hh"
#include "step/Steppable.hh"
#include "step/entity/EntityMoveStep.hh"
#include "utils/Vector.hh"

#include "StepAdditionVisitor.hh"

namespace octopus
{


Step::~Step()
{
	for(SteppableBundle &bundle_l : _listSteppable)
	{
		delete bundle_l._steppable;
	}
}

void Step::addEntityMoveStep(EntityMoveStep * step_p)
{
	_listEntityMoveStep.push_back(step_p);
	this->addSteppable(step_p);
}

void Step::addSteppable(Steppable * step_p)
{
	StepAdditionVisitor vis_l(*this);
	vis_l(step_p);
	_listSteppable.push_back(SteppableBundle{step_p, step_p->newData()});
}

std::list<EntityMoveStep *> &Step::getEntityMoveStep()
{
	return _listEntityMoveStep;
}

std::list<EntityMoveStep *> const &Step::getEntityMoveStep() const
{
	return _listEntityMoveStep;
}

std::list<SteppableBundle> &Step::getSteppable()
{
	return _listSteppable;
}

std::list<SteppableBundle> const &Step::getSteppable() const
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

unsigned long & Step::getDivOptionSpent(unsigned long player_p)
{
	return _divOptionsSpent[player_p];
}

unsigned long Step::getDivOptionSpent(unsigned long player_p) const
{
	auto &&itPlayer_l = _divOptionsSpent.find(player_p);
	if(itPlayer_l == _divOptionsSpent.end())
	{
		return 0;
	}
	return itPlayer_l->second;
}

void Step::addEntitySpawned()
{
	++_entitySpawned;
}

unsigned long Step::getEntitySpawned() const
{
	return _entitySpawned;
}

void apply(Step const & step_p, State &state_p)
{
	// apply all steppables
	for(SteppableBundle const &bundle_l: step_p.getSteppable())
	{
		bundle_l._steppable->apply(state_p, bundle_l._data);
	}
}

void revert(Step const & step_p, State &state_p)
{
	// apply all steppables (in reverse order)
	for(auto it_l = step_p.getSteppable().rbegin(); it_l != step_p.getSteppable().rend() ; ++it_l)
	{
		it_l->_steppable->revert(state_p, it_l->_data);
	}
}

void compact(Step & step_p)
{
	// first remove entity move step
	step_p.getEntityMoveStep().clear();

	for(auto it_l = step_p.getSteppable().begin() ; it_l != step_p.getSteppable().end() ; )
	{
		if(it_l->_steppable->isNoOp())
		{
			delete it_l->_steppable;
			delete it_l->_data;
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
	for(SteppableBundle const & steppable_l : step_p.getSteppable())
	{
		visitor_p(steppable_l._steppable);
	}
}

Handle getNextHandle(Step const &step_p, State const &state_p)
{
	return step_p.getEntitySpawned() + state_p.getEntities().size();
}

} // namespace octopus
