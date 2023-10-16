#include "LevelModel.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/model/entity/EntityModel.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"
#include "step/trigger/TriggerSpawn.hh"

#include "utils/EntitySpawner.h"
#include "utils/TriggerModel.h"

namespace godot {

LevelModel::~LevelModel()
{}

////////////////
/// player
////////////////
int LevelModel::add_player(int team)
{
    int idx_l = _players.size();
    unsigned long team_l = static_cast<unsigned long>(team);
    _players.push_back(GodotPlayer {{},{}, team_l});
    return idx_l;
}

void LevelModel::add_resource(int player, String const &resource, float quantity)
{
    std::string res_l(resource.utf8().get_data());
    _players.at(player).resources[res_l] += quantity;
}

void LevelModel::add_building(int player, String const &building)
{
    _players.at(player).buildings.push_back(building.utf8().get_data());
}

////////////////
/// player
////////////////
void LevelModel::add_entity(String const &type, String const &model, int player, float x, float y, PackedInt32Array const &array_p, int num_of_players)
{
    std::string type_l(type.utf8().get_data());
    std::string model_l(model.utf8().get_data());
    unsigned long player_l = static_cast<unsigned long>(player);
    unsigned long num_of_players_l = static_cast<unsigned long>(num_of_players);
	GodotEntity ent_l {type_l, model_l, player_l, x, y, {}, num_of_players_l};
	for(int i = 0 ; i < array_p.size() ; ++ i)
	{
		ent_l.entity_group.push_back(array_p[i]);
	}
    _entities.push_back(ent_l);
}

int LevelModel::add_trigger()
{
	_triggers.push_back(GodotTrigger());
	return _triggers.size()-1;
}

void LevelModel::add_trigger_listener_entity_dead_group(int triggerIdx_p, int entityDeadGroup_p)
{
	_triggers.at(triggerIdx_p).listeners.push_back(GodotTriggerListenerEntityDied {static_cast<unsigned long>(entityDeadGroup_p)});
}

void LevelModel::add_trigger_listener_timer(int triggerIdx_p, int steps_p)
{
	_triggers.at(triggerIdx_p).listeners.push_back(GodotTriggerListenerTimer {static_cast<unsigned long>(steps_p)});
}

void LevelModel::add_trigger_listener_zone_player(int triggerIdx_p, int player_p, String const &zone_p)
{
    std::string zone_name_l(zone_p.utf8().get_data());
	_triggers.at(triggerIdx_p).listeners.push_back(GodotTriggerZonePlayer {static_cast<unsigned long>(player_p), zone_name_l});
}

void LevelModel::add_trigger_listener_zone_team(int triggerIdx_p, int team_p, String const &zone_p)
{
    std::string zone_name_l(zone_p.utf8().get_data());
	_triggers.at(triggerIdx_p).listeners.push_back(GodotTriggerZoneTeam {static_cast<unsigned long>(team_p), zone_name_l});
}

void LevelModel::add_trigger_action_dialog(int triggerIdx_p, String const &dialogIdx_p, bool end_p, int team_winning_p)
{
    std::string dialog_l(dialogIdx_p.utf8().get_data());
	_triggers.at(triggerIdx_p).actions.push_back(GodotTriggerActionDialog {dialog_l, end_p, team_winning_p});
}

void LevelModel::add_trigger_action_spawn_entity(int triggerIdx_p, String const &type, String const &model, int player, float x, float y, int num_of_players)
{
    std::string type_l(type.utf8().get_data());
    std::string model_l(model.utf8().get_data());
    unsigned long player_l = static_cast<unsigned long>(player);
    unsigned long num_of_players_l = static_cast<unsigned long>(num_of_players);

	GodotTriggerActionSpawn actionSpawn_l;
	actionSpawn_l.entities_to_spawn.push_back(GodotEntity {type_l, model_l, player_l, x, y, {}, num_of_players_l});
	_triggers.at(triggerIdx_p).actions.push_back(actionSpawn_l);
}

void LevelModel::add_trigger_action_camera(int triggerIdx_p, int x, int y, int player_p)
{
	_triggers.at(triggerIdx_p).actions.push_back(GodotTriggerActionCamera {x, y, player_p});
}

void LevelModel::add_trigger_action_damage_zone(int triggerIdx_p, int damage_p, int team_p, String const &zone_name_p)
{
    std::string zone_name_l(zone_name_p.utf8().get_data());
	_triggers.at(triggerIdx_p).actions.push_back(GodotTriggerActionZoneDamage {damage_p, static_cast<unsigned long>(team_p), zone_name_l, octopus::Box<long>()});
}

void LevelModel::add_zone(String const &name_p, int x, int y, int size_x, int size_y)
{
    std::string name_l(name_p.utf8().get_data());
	_zones.push_back(GodotZone {name_l, octopus::Box<long>{x, x+size_x, y, y+size_y} } );
}

void LevelModel::_bind_methods()
{
    UtilityFunctions::print("Binding LevelModel methods");

    ClassDB::bind_method(D_METHOD("add_player", "team"), &LevelModel::add_player);
    ClassDB::bind_method(D_METHOD("add_resource", "player", "resource", "quantity"), &LevelModel::add_resource);
    ClassDB::bind_method(D_METHOD("add_building", "player", "building"), &LevelModel::add_building);

    ClassDB::bind_method(D_METHOD("add_entity", "type", "model", "player", "x", "y", "entity_group", "num_of_players"), &LevelModel::add_entity);

	ClassDB::bind_method(D_METHOD("add_trigger"), &LevelModel::add_trigger);
	ClassDB::bind_method(D_METHOD("add_trigger_listener_entity_dead_group", "trigger_idx", "entity_dead_group"), &LevelModel::add_trigger_listener_entity_dead_group);
	ClassDB::bind_method(D_METHOD("add_trigger_listener_timer", "trigger_idx", "steps"), &LevelModel::add_trigger_listener_timer);
	ClassDB::bind_method(D_METHOD("add_trigger_listener_zone_player", "trigger_idx", "player", "zone_name"), &LevelModel::add_trigger_listener_zone_player);
	ClassDB::bind_method(D_METHOD("add_trigger_listener_zone_team", "trigger_idx", "team", "zone_name"), &LevelModel::add_trigger_listener_zone_team);
	ClassDB::bind_method(D_METHOD("add_trigger_action_dialog", "trigger_idx", "dialog_idx", "end", "winning_team"), &LevelModel::add_trigger_action_dialog);
	ClassDB::bind_method(D_METHOD("add_trigger_action_spawn_entity", "trigger_idx", "type", "model", "player", "x", "y", "num_of_players"), &LevelModel::add_trigger_action_spawn_entity);
	ClassDB::bind_method(D_METHOD("add_trigger_action_camera", "trigger_idx", "x", "y", "player"), &LevelModel::add_trigger_action_camera);
	ClassDB::bind_method(D_METHOD("add_trigger_action_damage_zone", "trigger_idx", "damage", "team", "zone_name"), &LevelModel::add_trigger_action_damage_zone);


	ClassDB::bind_method(D_METHOD("add_zone", "name", "x", "y", "size_x", "size_y"), &LevelModel::add_zone);

    ADD_GROUP("LevelModel", "LevelModel_");
}

std::list<octopus::Steppable *> LevelModel::generateLevelSteps(octopus::Library const &lib_p, unsigned long playerCount_p)
{
    std::list<octopus::Steppable *> steps_l;

    for(unsigned long idx_l = 0 ; idx_l < _players.size() ; ++idx_l)
    {
        GodotPlayer const &player_l = _players.at(idx_l);
        steps_l.push_back(new octopus::PlayerSpawnStep(idx_l, player_l.team));

        // create resource spawn (need negative)
		steps_l.push_back(new octopus::PlayerSpendResourceStep(0, octopus::getReverseCostMap(player_l.resources)));

        for(std::string const &modelId_l : player_l.buildings)
        {
		    steps_l.push_back(new octopus::PlayerAddBuildingModel(0, lib_p.getBuildingModel(modelId_l)));
        }
    }

	unsigned long handle_l = 0;
    for(unsigned long idx_l = 0 ; idx_l < _entities.size() ; ++idx_l)
    {
        GodotEntity const &ent_l = _entities.at(idx_l);
		bool added_l = spawnEntity(steps_l, octopus::Handle(handle_l), ent_l, lib_p, playerCount_p);
		if(added_l)
		{
			++handle_l;
		}
    }

    for(unsigned long idx_l = 0 ; idx_l < _triggers.size() ; ++idx_l)
    {
		TriggerModel * model_l = newTriggerModel(_triggers[idx_l], _entities, lib_p, playerCount_p, _zones);
		// Can return null if trigger is not valid
		if(model_l)
		{
			steps_l.push_back(new octopus::TriggerSpawn(model_l));
		}
	}

    return steps_l;
}


}
