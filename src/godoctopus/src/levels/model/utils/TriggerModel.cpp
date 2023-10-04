#include "TriggerModel.h"


#include "controller/trigger/Listener.hh"
#include "step/Step.hh"

#include "controller/step/DialogStep.h"

#include "EntitySpawner.h"

namespace godot
{

TriggerModel * newTriggerModel(GodotTrigger const &trigger_p, std::vector<GodotEntity> const &entities_p,
	octopus::Library const &lib_p, unsigned long playerCount_p)
{
	std::list<octopus::Listener *> listeners_l;

	// create dead trigger
	if(trigger_p.entity_dead_trigger)
	{
		std::unordered_set<octopus::Handle> set_l;
		// for every entity in the group
		for(size_t i = 0 ; i < entities_p.size() ; ++i)
		{
			std::vector<unsigned long> const &entGroup_l = entities_p[i].entity_group;
			if(std::find(entGroup_l.begin(), entGroup_l.end(), trigger_p.entity_dead_group) != entGroup_l.end())
			{
				set_l.insert(octopus::Handle(i));
			}
		}

		octopus::ListenerEntityDied * listener_l = new octopus::ListenerEntityDied(set_l);
		listeners_l.push_back(listener_l);
	}

	unsigned long entity_dead_group = 0;
	TriggerModel * trigger_l = new TriggerModel(listeners_l ,trigger_p.action, lib_p, playerCount_p);
}

TriggerModel::TriggerModel(std::list<octopus::Listener *> const &listeners_p, GodotTriggerAction const &trigger_p, octopus::Library const &lib_p, unsigned long playerCount_p) :
	octopus::OneShotTrigger(listeners_p), _triggerAction(trigger_p), _lib(lib_p), _playerCount(playerCount_p)
{}

void TriggerModel::trigger(octopus::State const &state_p, octopus::Step &step_p, unsigned long count_p, octopus::TriggerData const &data_p) const
{
	if(_triggerAction.dialog_enabled)
	{
		step_p.addSteppable(new godot::DialogStep(_triggerAction.dialog_idx));
	}

	if(_triggerAction.unit_spawn)
	{
		for(GodotEntity const &ent_l : _triggerAction.entities_to_spawn)
		{
			std::list<octopus::Steppable *> list_l;
			spawnEntity(list_l, getNextHandle(step_p, state_p), ent_l, _lib, _playerCount);
		}
	}
}

} // namespace godot
