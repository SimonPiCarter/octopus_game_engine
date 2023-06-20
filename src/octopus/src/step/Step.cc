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

std::list<EntityMoveStep *> &Step::getEntityMoveStep()
{
	return _listEntityMoveStep;
}

std::list<EntityMoveStep *> const &Step::getEntityMoveStep() const
{
	return _listEntityMoveStep;
}

std::list<Steppable const *> &Step::getSteppable()
{
	return _listSteppable;
}

std::list<Steppable const *> const &Step::getSteppable() const
{
	return _listSteppable;
}

Fixed & Step::getResourceSpent(unsigned long player_p, std::string res_p)
{
	return _spent[player_p][res_p];
}
Fixed Step::getResourceSpent(unsigned long player_p, std::string res_p) const
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

const std::map<std::string, Fixed> & Step::getResourceSpent(unsigned long player_p) const
{
	/// @brief static empty map when no resources spent for player
	static std::map<std::string, Fixed> empty_l;
	auto &&itPlayer_l = _spent.find(player_p);
	if(itPlayer_l == _spent.end())
	{
		return empty_l;
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

void Step::updateProducedUpgrade(unsigned long const &player_p, std::string const &upgrade_p, bool add_p)
{
	if(add_p)
	{
		++_producedUpgrade[player_p][upgrade_p];
	}
	else
	{
		--_producedUpgrade[player_p][upgrade_p];
	}
}

bool Step::isUpgradeProduced(unsigned long const &player_p, std::string const &upgrade_p) const
{
	auto &&itPlayer_l = _producedUpgrade.find(player_p);
	if(itPlayer_l != _producedUpgrade.end())
	{
		std::unordered_map<std::string, long> map_l;
		auto &&itUpgrade_l = map_l.find(upgrade_p);
		if(itUpgrade_l != map_l.end())
		{
			return itUpgrade_l->second > 0;
		}
	}
	return false;
}


void Step::addCanceledBuilding(Handle const &handle_p)
{
	_canceledBuildings.insert(handle_p);
}

bool Step::isCanceled(Handle const &handle_p) const
{
	return _canceledBuildings.find(handle_p) != _canceledBuildings.end();
}

void Step::addAbilityRegistered(Handle const &handle_p, std::string const &key_p)
{
	_commandRegistered[handle_p].insert(key_p);
}

bool Step::checkAbilityNotAlreadyRegistered(Handle const &handle_p, std::string const &key_p) const
{
	auto &&it_l = _commandRegistered.find(handle_p);
	return it_l == _commandRegistered.end()
		|| it_l->second.find(key_p) == it_l->second.end();
}

void Step::addCmdCanceled(CommandIdx cmdidx_p)
{
	_canceledCmd.insert(cmdidx_p);
}

bool Step::isCmdCanceled(CommandIdx cmdidx_p) const
{
	return _canceledCmd.find(cmdidx_p) != _canceledCmd.end();
}

void Step::addHpChange(Handle const &handle_p, Fixed delta_p)
{
	_hpChange[handle_p] += delta_p;
}

std::unordered_map<Handle, Fixed> const& Step::getHpChange() const
{
	return _hpChange;
}

Fixed Step::getHpChange(Handle const &handle_p) const
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
	state_p.addStepApplied();
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
	size_t i = vecData_l.size();
	// apply all steppables (in reverse order)
	for(auto it_l = step_p.getSteppable().rbegin() ; it_l != step_p.getSteppable().rend() ; ++it_l)
	{
		// apply steppable with state data
		(*it_l)->revert(state_p, vecData_l[i-1]);

		// delete data
		delete *it_l;
		--i;
	}

	vecData_l.clear();
	state_p.removeStepApplied();
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
