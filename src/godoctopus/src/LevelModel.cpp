#include "LevelModel.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

// octopus
#include "state/model/entity/EntityModel.hh"
#include "step/entity/spawn/BuildingSpawnStep.hh"
#include "step/state/StateTemplePositionAddStep.hh"
#include "step/entity/spawn/ResourceSpawnStep.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerAddBuildingModel.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "step/player/PlayerSpendResourceStep.hh"

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
void LevelModel::add_entity(String const &type, String const &model, int player, float x, float y)
{
    std::string type_l(type.utf8().get_data());
    std::string model_l(model.utf8().get_data());
    unsigned long player_l = static_cast<unsigned long>(player);
    _entities.push_back(GodotEntity {type_l, model_l, player_l, x, y});
}

void LevelModel::_bind_methods()
{
    UtilityFunctions::print("Binding LevelModel methods");

    ClassDB::bind_method(D_METHOD("add_player", "team"), &LevelModel::add_player);
    ClassDB::bind_method(D_METHOD("add_resource", "player", "resource", "quantity"), &LevelModel::add_resource);
    ClassDB::bind_method(D_METHOD("add_building", "player", "building"), &LevelModel::add_building);

    ClassDB::bind_method(D_METHOD("add_entity", "type", "model", "player", "x", "y"), &LevelModel::add_entity);

    ADD_GROUP("LevelModel", "LevelModel_");
}

std::list<octopus::Steppable *> LevelModel::generateLevelSteps(octopus::Library const &lib_p)
{
    std::list<octopus::Steppable *> steps_l;

    for(unsigned long idx_l = 0 ; idx_l < _players.size() ; ++idx_l)
    {
        GodotPlayer const &player_l = _players.at(idx_l);
        steps_l.push_back(new octopus::PlayerSpawnStep(octopus::Handle(idx_l), player_l.team));

        // create resource spawn (need negative)
		steps_l.push_back(new octopus::PlayerSpendResourceStep(0, octopus::getReverseCostMap(player_l.resources)));

        for(std::string const &modelId_l : player_l.buildings)
        {
		    steps_l.push_back(new octopus::PlayerAddBuildingModel(0, lib_p.getBuildingModel(modelId_l)));
        }
    }

    for(unsigned long idx_l = 0 ; idx_l < _entities.size() ; ++idx_l)
    {
        GodotEntity const &ent_l = _entities.at(idx_l);
        if(ent_l.type == "Unit")
        {
	        octopus::Unit unit_l({ ent_l.x, ent_l.y }, false, lib_p.getUnitModel(ent_l.model));
            unit_l._player = ent_l.player;
            steps_l.push_back(new octopus::UnitSpawnStep(octopus::Handle(idx_l), unit_l));
        }
        else if(ent_l.type == "Building")
        {
	        octopus::Building building_l({ ent_l.x, ent_l.y }, true, lib_p.getBuildingModel(ent_l.model));
            building_l._player = ent_l.player;
            steps_l.push_back(new octopus::BuildingSpawnStep(octopus::Handle(idx_l), building_l, true));
        }
        else if(ent_l.type == "Resource")
        {
	        octopus::Resource resource_l({ ent_l.x, ent_l.y }, true, lib_p.getEntityModel(ent_l.model));
			resource_l._resource = 20;
            resource_l._player = ent_l.player;
            steps_l.push_back(new octopus::ResourceSpawnStep(octopus::Handle(idx_l), resource_l));
        }
        else if(ent_l.type == "Anchor")
        {
	        octopus::Building building_l({ ent_l.x, ent_l.y }, true, lib_p.getBuildingModel(ent_l.model));
            building_l._player = ent_l.player;
            steps_l.push_back(new octopus::BuildingSpawnStep(octopus::Handle(idx_l), building_l, true));
            steps_l.push_back(new octopus::StateTemplePositionAddStep({ ent_l.x, ent_l.y }));
        }
        else
        {
            throw std::logic_error("Unkown type found in entities when generating level model");
        }
    }

    return steps_l;
}


}
