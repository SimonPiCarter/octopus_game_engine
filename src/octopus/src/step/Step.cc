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
	for(Steppable const *steppable_l : _listSteppable)
	{
		delete steppable_l;
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
	_listSteppable.push_back(step_p);
}

std::vector<EntityMoveStep *> &Step::getEntityMoveStep()
{
	return _listEntityMoveStep;
}

std::vector<EntityMoveStep *> const &Step::getEntityMoveStep() const
{
	return _listEntityMoveStep;
}

std::vector<Steppable const *> &Step::getSteppable()
{
	return _listSteppable;
}

std::vector<Steppable const *> const &Step::getSteppable() const
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

void Step::addFlyingCommnandSpawned()
{
	++_flyingCommandSpawned;
}

unsigned long Step::getFlyingCommandSpawned() const
{
	return _flyingCommandSpawned;
}

void Step::addCanceledBuilding(Handle const &handle_p)
{
	_canceledBuildings.insert(handle_p);
}

bool Step::isCanceled(Handle const &handle_p) const
{
	return _canceledBuildings.find(handle_p) != _canceledBuildings.end();
}

void Step::addCmdCanceled(CommandData const *data_p)
{
	_canceledCmd.insert(data_p);
}

bool Step::isCmdCanceled(CommandData const *data_p) const
{
	return _canceledCmd.find(data_p) != _canceledCmd.end();
}

void Step::addHpChange(Handle const &handle_p, double delta_p)
{
	_hpChange[handle_p] += delta_p;
}

std::unordered_map<Handle, double> const& Step::getHpChange() const
{
	return _hpChange;
}

double Step::getHpChange(Handle const &handle_p) const
{
	auto it_l = _hpChange.find(handle_p);
	if(it_l != _hpChange.end())
	{
		return it_l->second;
	}
	return 0;
}

StepData::~StepData()
{
	for(SteppableData * data_l : _listSteppableData)
	{
		delete data_l;
	}
}

void apply(Step const & step_p, State &state_p, StepData &stepData_p)
{
	bool buildData_l = stepData_p._listSteppableData.empty();
	if(buildData_l)
	{
		stepData_p._listSteppableData.reserve(step_p.getSteppable().size());
	}

	// apply all steppables
	for(Steppable const *steppable_l : step_p.getSteppable())
	{
		if(buildData_l)
		{
			stepData_p._listSteppableData.push_back(steppable_l->newData(state_p));
		}
		// apply steppable with state data
		steppable_l->apply(state_p);
	}
}

void revert(Step const & step_p, State &state_p, StepData &stepData_p)
{
	std::vector<SteppableData *> &vecData_l = stepData_p._listSteppableData;
	if(vecData_l.empty())
	{
		throw std::logic_error("tried to revert the same step twice to a state or reverted a step without applying it first");
	}
	// apply all steppables (in reverse order)
	for(size_t i = step_p.getSteppable().size() ; i > 0 ; --i)
	{
		// apply steppable with state data
		step_p.getSteppable()[i-1]->revert(state_p, vecData_l[i-1]);

		// delete data
		delete vecData_l[i-1];
	}

	vecData_l.clear();
}

void compact(Step & step_p)
{
	// first remove entity move step
	step_p.getEntityMoveStep().clear();

	for(auto it_l = step_p.getSteppable().begin() ; it_l != step_p.getSteppable().end() ; )
	{
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
	for(Steppable const *steppable_l : step_p.getSteppable())
	{
		visitor_p(steppable_l);
	}
}

Handle getNextHandle(Step const &step_p, State const &state_p)
{
	return step_p.getEntitySpawned() + state_p.getEntities().size();
}

} // namespace octopus
