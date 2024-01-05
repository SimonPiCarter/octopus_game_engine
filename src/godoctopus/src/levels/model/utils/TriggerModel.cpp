#include "TriggerModel.h"


#include "controller/trigger/Listener.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "step/Step.hh"
#include "step/command/CommandQueueStep.hh"
#include "step/state/StateWinStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/entity/EntityHitPointChangeStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "command/entity/EntityAttackMoveCommand.hh"

#include "controller/step/DialogStep.h"
#include "controller/step/CameraStep.h"
#include "controller/step/ObjectiveStep.h"

#include "EntitySpawner.h"
#include "utils/Box.hh"

namespace godot
{

struct GodotListenerVisitor
{
	GodotListenerVisitor(std::list<octopus::Listener *> &list_r, std::vector<GodotEntity> const &entities_p,
		octopus::Library const &lib_p, unsigned long playerCount_p, std::vector<GodotZone> const &zones_p) :
		_list(list_r),
		_entities(entities_p),
		_lib(lib_p),
		_playerCount(playerCount_p)
	{
		for(GodotZone const &zone_l : zones_p)
		{
			_mapZone[zone_l.name] = zone_l.zone;
		}
	}

	void operator()(GodotTriggerListenerEntityDied const &listener_p)
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

		_emptyEntityGroupDead |= set_l.empty();

		octopus::ListenerEntityDied * listener_l = new octopus::ListenerEntityDied(set_l);
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerListenerTimer const &listener_p) const
	{
		octopus::ListenerStepCount * listener_l = new octopus::ListenerStepCount(listener_p.steps);
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerZonePlayer const &listener_p) const
	{
		octopus::ListenerZone * listener_l = octopus::ListenerZone::newListenerZonePlayer(listener_p.player, _mapZone.at(listener_p.zone_name));
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerZoneTeam const &listener_p) const
	{
		octopus::ListenerZone * listener_l = octopus::ListenerZone::newListenerZoneTeam(listener_p.team, _mapZone.at(listener_p.zone_name));
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerListenerResource const &listener_p) const
	{
		if(listener_p.lower_than)
		{
			octopus::ListenerResource<false> * listener_l = new octopus::ListenerResource<false>(listener_p.player, listener_p.resource, listener_p.qty);
			_list.push_back(listener_l);
		}
		else
		{
			octopus::ListenerResource<true> * listener_l = new octopus::ListenerResource<true>(listener_p.player, listener_p.resource, listener_p.qty);
			_list.push_back(listener_l);
		}
	}

	void operator()(GodotTriggerEntityProduced const &listener_p) const
	{
		octopus::ListenerEntityModelFinished * listener_l = new octopus::ListenerEntityModelFinished(&_lib.getEntityModel(listener_p.model), listener_p.player);
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerUpgrade const &listener_p) const
	{
		octopus::ListenerUpgrade * listener_l = new octopus::ListenerUpgrade(listener_p.player, listener_p.upgrade, listener_p.level);
		_list.push_back(listener_l);
	}

	void operator()(GodotTriggerActionDialog &) const {}
	void operator()(GodotTriggerActionSpawn &) const {}
	void operator()(GodotTriggerActionCamera &) const {}
	void operator()(GodotTriggerActionZoneDamage &action_p) const
	{
		action_p.zone = _mapZone.at(action_p.zone_name);
	}
	void operator()(GodotTriggerActionAddObjective&) const {}
	void operator()(GodotTriggerActionCompleteObjective&) const {}
	void operator()(GodotTriggerActionFailObjective&) const {}
	void operator()(GodotTriggerActionIncrementObjective&) const {}
	void operator()(GodotTriggerActionResource&) const {}

	std::list<octopus::Listener *> &_list;
	std::vector<GodotEntity> const &_entities;
	octopus::Library const &_lib;
	unsigned long const _playerCount;
	std::map<std::string, octopus::Box<long> > _mapZone;

	bool _emptyEntityGroupDead = false;
};

TriggerModel * newTriggerModel(GodotTrigger &trigger_p, std::vector<GodotEntity> const &entities_p,
	octopus::Library const &lib_p, unsigned long playerCount_p, std::vector<GodotZone> const &zones_p)
{
	std::list<octopus::Listener *> listeners_l;
	GodotListenerVisitor vis_l(listeners_l, entities_p, lib_p, playerCount_p, zones_p);
	// create listeners
	for(GodotTriggerListener const &listener_l : trigger_p.listeners)
	{
   		std::visit([&](auto &&arg) { vis_l(arg); }, listener_l);
	}
	// handle actions (fill up data if required)
	for(GodotTriggerAction &action_l : trigger_p.actions)
	{
   		std::visit([&](auto &&arg) { vis_l(arg); }, action_l);
	}

	if(vis_l._emptyEntityGroupDead)
	{
		return nullptr;
	}

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
		if(action_p.end)
		{
			_step.addSteppable(new octopus::StateWinStep(false, false, 0, action_p.winning_team));
		}
		_step.addSteppable(new godot::DialogStep(action_p.dialog_idx));
	}
	void operator()(GodotTriggerActionSpawn const &action_p) const
	{
		for(GodotEntity const &ent_l : action_p.entities_to_spawn)
		{
			std::list<octopus::Steppable *> list_l;
			octopus::Handle handle_l = getNextHandle(_step, _state);
			spawnEntity(list_l, handle_l, ent_l, _lib, _playerCount);
			for(octopus::Steppable *steppable_l : list_l)
			{
				_step.addSteppable(steppable_l);
			}
			if(action_p.attack_move)
			{
				_step.addSteppable(new octopus::CommandSpawnStep(
					new octopus::EntityAttackMoveCommand(handle_l, handle_l, octopus::Vector(action_p.x, action_p.y),
						0, {octopus::Vector(action_p.x, action_p.y)}, true, false)));
			}
		}
	}

	void operator()(GodotTriggerActionCamera const &action_p) const
	{
		_step.addSteppable(new godot::CameraStep(action_p.x, action_p.y, action_p.player));
	}

	void operator()(GodotTriggerActionZoneDamage const &action_p) const
	{
		octopus::Box<octopus::Fixed> box_l {action_p.zone._lowerX, action_p.zone._upperX, action_p.zone._lowerY, action_p.zone._upperY};
		std::vector<octopus::Entity const *> entities_l = octopus::getAllEntitiesInBox(box_l, _state, false);
		for(octopus::Entity const * ent_l : entities_l)
		{
			octopus::Player const * player_l = _state.getPlayer(ent_l->_player);

			if(ent_l->_pos.x < action_p.zone._lowerX || ent_l->_pos.x > action_p.zone._upperX
			|| ent_l->_pos.y < action_p.zone._lowerY || ent_l->_pos.y > action_p.zone._upperY)
			{
				continue;
			}

			if(player_l->_team == action_p.team)
			{
				octopus::Fixed curHp_l = ent_l->_hp + _step.getHpChange(ent_l->_handle);
				octopus::Fixed maxHp_l = ent_l->getHpMax();
				_step.addSteppable(new octopus::EntityHitPointChangeStep(ent_l->_handle, -action_p.damage, curHp_l, maxHp_l));
			}
		}
	}

	void operator()(GodotTriggerActionAddObjective const &action_p) const
	{
		if(action_p.remove)
		{
			_step.addSteppable(new RemoveObjectiveStep(action_p.obj_name));
		}
		else
		{
			_step.addSteppable(new AddObjectiveStep(action_p.obj_name, action_p.obj_name, action_p.count, action_p.is_main));
		}
	}

	void operator()(GodotTriggerActionCompleteObjective const &action_p) const
	{
		_step.addSteppable(new CompleteObjectiveStep(action_p.obj_name, action_p.complete));
	}

	void operator()(GodotTriggerActionFailObjective const &action_p) const
	{
		_step.addSteppable(new FailObjectiveStep(action_p.obj_name, action_p.fail));
	}

	void operator()(GodotTriggerActionIncrementObjective const &action_p) const
	{
		_step.addSteppable(new IncrementObjectiveStep(action_p.obj_name, action_p.increment));
	}

	void operator()(GodotTriggerActionResource const &action_p) const
	{
		std::map<std::string, octopus::Fixed> resources_l;
		resources_l[action_p.resource] = octopus::Fixed(-action_p.qty);
		_step.addSteppable(new octopus::PlayerSpendResourceStep(action_p.player, resources_l));
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
