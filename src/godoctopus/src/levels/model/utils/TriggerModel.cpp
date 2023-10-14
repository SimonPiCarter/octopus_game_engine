#include "TriggerModel.h"


#include "controller/trigger/Listener.hh"
#include "step/Step.hh"

#include "controller/step/DialogStep.h"

#include "EntitySpawner.h"

namespace godot
{

struct GodotListenerVisitor
{
	GodotListenerVisitor(std::list<octopus::Listener *> &list_r, std::vector<GodotEntity> const &entities_p,
		octopus::Library const &lib_p, unsigned long playerCount_p) :
		_list(list_r),
		_entities(entities_p),
		_lib(lib_p),
		_playerCount(playerCount_p)
	{}

	void operator()(GodotTriggerListenerEntityDied const &listener_p) const
	{
		std::unordered_set<octopus::Handle> set_l;
		unsigned long handle_l = 0;
		// for every entity in the group
		for(size_t i = 0 ; i < _entities.size() ; ++i)
		{
			if(_entities[i].num_players_to_spawn > _playerCount)
			{
				continue;
			}
			std::vector<unsigned long> const &entGroup_l = _entities[i].entity_group;
			if(std::find(entGroup_l.begin(), entGroup_l.end(), listener_p.entity_group) != entGroup_l.end())
			{
				set_l.insert(octopus::Handle(handle_l));
			}
			++handle_l;
		}

		octopus::ListenerEntityDied * listener_l = new octopus::ListenerEntityDied(set_l);
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerListenerTimer const &listener_p) const
	{
		octopus::ListenerStepCount * listener_l = new octopus::ListenerStepCount(listener_p.steps);
		_list.push_back(listener_l);
	}

	std::list<octopus::Listener *> &_list;
	std::vector<GodotEntity> const &_entities;
	octopus::Library const &_lib;
	unsigned long const _playerCount;
};

TriggerModel * newTriggerModel(GodotTrigger const &trigger_p, std::vector<GodotEntity> const &entities_p,
	octopus::Library const &lib_p, unsigned long playerCount_p)
{
	std::list<octopus::Listener *> listeners_l;
	// create dead trigger
	for(GodotTriggerListener const &listener_l : trigger_p.listeners)
	{
		GodotListenerVisitor vis_l(listeners_l, entities_p, lib_p, playerCount_p);
   		std::visit([&](auto &&arg) { vis_l(arg); }, listener_l);
	}

	unsigned long entity_dead_group = 0;
	TriggerModel * trigger_l = new TriggerModel(listeners_l, trigger_p.actions, lib_p, playerCount_p);
	return trigger_l;
}

TriggerModel::TriggerModel(std::list<octopus::Listener *> const &listeners_p, std::vector<GodotTriggerAction> const &actions_p, octopus::Library const &lib_p, unsigned long playerCount_p) :
	octopus::OneShotTrigger(listeners_p), _actions(actions_p), _lib(lib_p), _playerCount(playerCount_p)
{}

struct GodotActionVisitor
{
	GodotActionVisitor(octopus::State const &state_p, octopus::Step &step_p,
		octopus::Library const &lib_p, unsigned long playerCount_p) :
		_state(state_p),
		_step(step_p),
		_lib(lib_p),
		_playerCount(playerCount_p)
	{}

	void operator()(GodotTriggerActionDialog const &action_p) const
	{
		_step.addSteppable(new godot::DialogStep(action_p.dialog_idx));
	}

	void operator()(GodotTriggerActionSpawn const &action_p) const
	{
		for(GodotEntity const &ent_l : action_p.entities_to_spawn)
		{
			std::list<octopus::Steppable *> list_l;
			spawnEntity(list_l, getNextHandle(_step, _state), ent_l, _lib, _playerCount);
			for(octopus::Steppable *steppable_l : list_l)
			{
				_step.addSteppable(steppable_l);
			}
		}
	}

	octopus::State const &_state;
	octopus::Step &_step;
	octopus::Library const &_lib;
	unsigned long const _playerCount;
};

void TriggerModel::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long count_p, octopus::TriggerData const &data_p) const
{
	GodotActionVisitor vis_l(state_p, step_p, _lib, _playerCount);

	for(GodotTriggerAction const &action_l : _actions)
	{
   		std::visit([&](auto &&arg) { vis_l(arg); }, action_l);
	}
}

} // namespace godot
